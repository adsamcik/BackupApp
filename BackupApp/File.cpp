#include "stdafx.h"
#include "File.h"
#include <istream>
#include <fstream>
#include <ctime>
#include <time.h> 
#ifdef _WIN32
#include <sys/utime.h>
#else
#include <utime.h>
#endif

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
	char* mLengthData = new char[sizeof(uint32_t)];
	stream.read(mLengthData, sizeof(uint32_t));
	uint32_t sLength = uint32_t(*reinterpret_cast<uint32_t*>(mLengthData));
	delete[] mLengthData;
	path = nullptr;
	//path is loaded on demand
	stream.seekg(sLength, std::ios::cur);
	//Load time
	//Some weird shit here
	char* mTimeData = new char[sizeof(time_t)];
	if (stream.read(mTimeData, sizeof(time_t)).fail()) {
		delete[] mTimeData;
		throw std::runtime_error("Failed to load last edit information");
	}
	auto t = reinterpret_cast<time_t*>(mTimeData);
	lastEdited = new tm();
#ifdef  _WIN32
	gmtime_s(lastEdited, t);
#else
	gmtime_r(t, lastEdited);
#endif

	beginContent = BHEADER_SIZE + sLength;

	//Load content begin and end
	stream.read(reinterpret_cast<char*>(&endContent), sizeof(endContent));
	stream.read(reinterpret_cast<char*>(&reserve), sizeof(reserve));

	delete[] mTimeData;
}

File::~File() {
	if (lastEdited != nullptr)
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

ext::Success File::Restore(std::fstream& stream) const {
	if (!stream.is_open())
		return ext::Success(false, "Source stream is not open");
	std::ofstream outfile;
	char* path = GetPath();
	if (ext::isValidPath(path))
		if (remove(path) != 0)
			return ext::Success(false, "Failed to restore the file. Current file cannot be rewritten.");
	outfile.open(path);
	if (!outfile.is_open()) {
		delete[] path;
		return ext::Success(false, "Target file could not be created.");
	}

	auto length = static_cast<long long>(endContent - beginContent - reserve);
	auto count = length / BUFFER_SIZE;
	char *cache = new char[BUFFER_SIZE];
	stream.seekg(beginMeta + beginContent);
	for (long long i = 0; i < count; i++) {
		stream.read(cache, BUFFER_SIZE);
		outfile.write(cache, BUFFER_SIZE);
	}
	delete[] cache;
	auto diff = length - (count * BUFFER_SIZE);
	if (diff > 0) {
		char *temp = new char[diff];
		stream.read(temp, diff);
		outfile.write(temp, diff);
		delete[] temp;
	}
	outfile.close();
	struct utimbuf ut;
	ut.modtime = timegm(lastEdited);
	ut.actime = timegm(lastEdited);
	if (utime(path, &ut) == -1)
		std::cout << "Failed to modify file time" << std::endl;
	std::cout << "Restored " << path << std::endl;
	delete[] path;
	return ext::Success();
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
