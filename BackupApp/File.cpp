#include "stdafx.h"
#include "File.h"
#include <istream>
#include <fstream>
#include <ctime>

File::File(const std::string & path) {
	if (!ext::isValidPath(path))
		throw std::exception("path is invalid!");
	this->path = new std::string(path);

	struct stat t_stat;
	auto r = stat(path.c_str(), &t_stat);
	this->lastEdited = new tm();
	gmtime_s(this->lastEdited, &t_stat.st_mtime);
}

File::File(std::fstream & stream, const std::streampos & begin) {
	char* mLong = new char[8];
	char* mInt = new char[4];
	this->beginMeta = begin;
	//Load string
	stream.get(mInt, 4);
	size_t sLength = reinterpret_cast<size_t>(mInt);
	//path is loaded on demand
	stream.seekg(sLength, std::ios::cur);
	//Load time
	stream.get(mLong, 4);
	auto t = reinterpret_cast<time_t>(mLong);
	lastEdited = new tm();
	gmtime_s(lastEdited, &t);

	//Load content begin and end
	stream.get(mLong, 8);
	beginContent = reinterpret_cast<long long>(mLong);
	stream.get(mLong, 8);
	endContent = reinterpret_cast<long long>(mLong);

	delete[] mLong;
	delete[] mInt;
}

File::~File() {
	delete lastEdited;
	delete path;
}

void File::Restore(std::fstream& stream) const {
	std::ofstream outfile;
	outfile.open(GetPath());
	auto size = static_cast<long long>(endContent - beginContent);
	auto count = size / 32;
	char* cache = new char[32];
	for (long long i = 0; i < count; i++) {
		stream.get(cache, 32);
		outfile << cache;
	}
	delete[] cache;
	auto diff = size - (count * 32);
	if (diff > 0) {
		char* temp = new char[diff];
		stream.get(temp, diff);
		outfile << temp;
		delete[] temp;
	}
	outfile.close();
}

bool File::IsValid() const {
	return ext::isValidPath(GetPath());
}

std::string* File::GetPath() {
	if (path == nullptr) {
		std::ifstream is(BACKUP_FILE);
		is.seekg(beginMeta);
		char* buff = new char[4];
		is.read(buff, 4);
		auto length = *reinterpret_cast<int*>(buff);
		delete[] buff;
		buff = new char[length];
		is.read(buff, length);
		path = new std::string(buff);
		is.close();
	}
	return path;
}

std::string File::GetPath() const {
	if (path == nullptr) {
		std::ifstream is(BACKUP_FILE);
		is.seekg(beginMeta);
		char* buff = new char[4];
		is.read(buff, 4);
		auto length = *reinterpret_cast<int*>(buff);
		delete[] buff;
		buff = new char[length];
		is.read(buff, length);
		return std::string(buff);
		is.close();
	}
	return *path;
}

void File::ClearPath() {
	if (path != nullptr)
		delete path;
	path = nullptr;
}

Dir::Dir(const std::string & path) :File(path) {}

void Dir::Restore(std::fstream & stream) const {

}

std::vector<std::string>* Dir::GetFiles() {
	DIR *dir;
	struct dirent *ent;
	auto v = new std::vector<std::string>();

	dir = opendir(GetPath()->c_str());
	//skip .
	readdir(dir);
	//skip ..
	readdir(dir);
	if (dir != NULL) {
		while (ent = readdir(dir)) {
			v->push_back(std::string(ent->d_name, ent->d_namlen));
		}
	}
	closedir(dir);
	return v;
}
