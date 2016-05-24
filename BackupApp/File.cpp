#include "stdafx.h"
#include "File.h"
#define FILE ".BACKP.cache"


File::File(const std::streampos & begin, const std::streampos & end) {
	this->begin = begin;
	this->end = end;
	std::ifstream stream(FILE);
	stream.seekg(begin);
	stream.read()
}

File::~File() {}
