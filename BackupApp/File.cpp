#include "stdafx.h"
#include "File.h"
#include <istream>
#include <fstream>
#include <ctime>

File::File(const std::string & path) {
	if (!ext::isValidPath(path))
		throw std::exception("path is invalid!");
	this->path = path;

	struct stat t_stat;
	auto r = stat(path.c_str(), &t_stat);
	gmtime_s(this->lastEdited, &t_stat.st_mtime);
}

File::File(std::fstream & stream, const std::streampos & beginMeta) {
	char* mLong = new char[8];
	char* mInt = new char[4];
	//Load string
	stream.get(mInt, 4);
	size_t sLength = reinterpret_cast<size_t>(mInt);
	char* string = new char[sLength];
	stream.get(string, sLength);
	path = string;
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
}

void File::Restore(std::fstream& stream) {
	std::ofstream outfile;
	outfile.open(path);
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

bool File::IsValid() {
	return ext::isValidPath(path);
}

Dir::Dir(const std::string & path) :File(path) {
	this->path = path;
}

void Dir::Restore(std::fstream & stream) {

}

std::vector<std::string>* Dir::GetFiles() {
	DIR *dir;
	struct dirent *ent;
	auto v = new std::vector<std::string>();

	dir = opendir(path.c_str());
	if (dir != NULL) {
		while (ent = readdir(dir)) {
			v->push_back(std::string(ent->d_name, ent->d_namlen));
		}
	}
	closedir(dir);
	return v;
}
