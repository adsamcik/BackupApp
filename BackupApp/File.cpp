#include "stdafx.h"
#include "File.h"
#include <istream>
#include <fstream>
#include <ctime>
#include <time.h> 

File::File(const std::string &path) {
	if (!ext::isValidPath(path))
		throw std::runtime_error("path is invalid!");
	this->path = new std::string(path);

	struct stat t_stat;
	//todo
	stat(path.c_str(), &t_stat);
	this->lastEdited = new tm();
#ifdef  _WIN32
	gmtime_s(this->lastEdited, &t_stat.st_mtime);
#else
	gmtime_r(&t_stat.st_mtime, this->lastEdited);
#endif
	this->beginMeta = -1;
	this->beginContent = -1;
	this->endContent = -1;
	this->reserve = -1;
}

File::File(std::fstream &stream) {
	this->beginMeta = stream.tellg();
	//Load string
	char* mLengthData = new char[4];
	stream.get(mLengthData, 4);
	uint32_t sLength = uint32_t(*reinterpret_cast<uint32_t*>(mLengthData));
	path = nullptr;
	//path is loaded on demand
	stream.seekg(sLength + 1, std::ios::cur);
	//Load time
	//Some weird shit here
	char* mTimeData = new char[9];
	if (stream.get(mTimeData, 9).fail())
		throw std::runtime_error("Failed to load last edit information");
	auto t = reinterpret_cast<time_t*>(mTimeData);
	lastEdited = new tm();
#ifdef  _WIN32
	gmtime_s(lastEdited, t);
#else
	gmtime_r(t, lastEdited);
#endif


	//Load content begin and end
	stream.get(reinterpret_cast<char*>(&endContent), sizeof(endContent));
	stream.get(reinterpret_cast<char*>(&reserve), sizeof(reserve));

	beginContent = BHEADER_SIZE + sLength;

	delete[] mTimeData;
	delete[] mLengthData;
}

File::~File() {
	delete lastEdited;
	ClearPath();
}

bool File::IsNewer() {
	struct stat s;
	if (stat(GetPath()->c_str(), &s) != 0)
		return false;
#ifdef DBG
	auto t = new tm();
	gmtime_s(t, &s.st_mtime);
	std::cout << "Comparing time of " << *GetPath() << std::endl << s.st_mtime << " new vs old " << timegm(lastEdited) << std::endl;
	char* buffer = new char[80];
	auto ptr = strftime(buffer, 80, "%c", t);
	std::cout << "new " << buffer << std::endl;
	ptr = strftime(buffer, 80, "%c", lastEdited);
	std::cout << "old " << buffer << std::endl;
	delete[] buffer;
	delete t;
#endif
	return s.st_mtime > timegm(lastEdited);
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
	struct stat t_stat;
	if (stat(path->c_str(), &t_stat) != 0)
		throw std::runtime_error("Failed to open source file");
	//path big or bigger than 4GB is unimaginable
	uint32_t size = static_cast<uint32_t>(GetPath()->size());
	stream->write(reinterpret_cast<char*>(&size), sizeof(size));
	stream->write(GetPath()->c_str(), GetPath()->length());
	stream->write(reinterpret_cast<char*>(&t_stat.st_mtime), sizeof(t_stat.st_mtime));
	stream->write(reinterpret_cast<char*>(&endContent), sizeof(endContent));
	stream->write(reinterpret_cast<char*>(&reserve), sizeof(reserve));
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
		if (is.read(buff, 4).fail())
			throw std::runtime_error("Failed to read length");
		auto length = *reinterpret_cast<int*>(buff);
		delete[] buff;
		if (length <= 0)
			throw std::runtime_error("Invalid length of string");
		buff = new char[length + 1];
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
	if (dir != NULL) {
		while ((ent = readdir(dir))) {
			string name = std::string(ent->d_name);
			if (name == "." || name == "..")
				continue;
			v->push_back(name);
		}
	}
	closedir(dir);
	std::sort(v->begin(), v->end());
	return v;
}
