#include "stdafx.h"
#include "File.h"
#include <istream>
#include <fstream>
#include <ctime>

File::File(const std::string &path) {
	if (!ext::isValidPath(path))
		throw std::exception("path is invalid!");
	this->path = new std::string(path);

	struct stat t_stat;
	auto r = stat(path.c_str(), &t_stat);
	this->lastEdited = new tm();
	gmtime_s(this->lastEdited, &t_stat.st_mtime);
	this->beginMeta = -1;
	this->beginContent = -1;
	this->endContent = -1;
}

File::File(std::fstream &stream) {
	this->beginMeta = stream.tellg();
	//Load string
	char* mLengthData = new char[4];
	stream.get(mLengthData, 4);
	uint32_t sLength = uint32_t(*reinterpret_cast<uint32_t*>(mLengthData));
	//path is loaded on demand
	stream.seekg(sLength+1, std::ios::cur);
	//Load time
	char* mTimeData = new char[9];
	stream.get(mTimeData, 9);
	auto t = reinterpret_cast<time_t*>(mTimeData);
	lastEdited = new tm();
	gmtime_s(lastEdited, t);

	//Load content begin and end
	stream.get(reinterpret_cast<char*>(&endContent), sizeof(endContent));

	beginContent = sizeof(sLength) + sLength + sizeof(time_t) + sizeof(std::streamoff);

	auto x = 0;

	delete[] mTimeData;
	delete[] mLengthData;
}

File::~File() {
	delete lastEdited;
	ClearPath();
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

void File::WriteMeta(std::fstream *stream) {
	auto path = GetPath();
	//path big or bigger than 4GB is unimaginable
	uint32_t size = static_cast<uint32_t>(path->size());
	stream->write(reinterpret_cast<char*>(&size), sizeof(size));
	stream->write(path->c_str(), path->length());
	auto le = mktime(lastEdited);
	stream->write(reinterpret_cast<char*>(&le), sizeof(le));
	stream->write(reinterpret_cast<char*>(&endContent), sizeof(endContent));
}

bool File::IsValid() const {
	bool valid = ext::isValidPath(GetPath());
	delete[] GetPath();
	return valid;
}

char* File::GetPath() const {
	if (path == nullptr) {
		std::ifstream is(BACKUP_FILE);
		is.seekg(beginMeta);
		char* buff = new char[4];
		is.read(buff, 4);
		auto length = *reinterpret_cast<int*>(buff);
		delete[] buff;
		if (length < 0 || length == 0xcdcdcdcd)
			throw std::exception("Error occured during loading of length");
		buff = new char[length+1];
		buff[length] = '\0';
		is.read(buff, length);
		is.close();
		return buff;
	}
	char *writable = new char[path->size() + 1];
	std::strcpy(writable, path->c_str());
	return writable;
}

std::string* File::GetPath() {
	if (path == nullptr) {
		const File* f = this;
		char* buff = f->GetPath();
		path = new std::string(buff);
		delete[] buff;
	}
	return path;
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
