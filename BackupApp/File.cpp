#include "stdafx.h"
#include "File.h"
#define FILE ".BACKP.cache"

/*
	FILE STRUCTURE
	--begin <-
	2B - length of path (LP)
	LPB - path			#path
	36B - date time		#time
	--beginContent <-
	THE REST - content
	--end <-
*/


File::File(const std::streampos & begin, const std::streampos & end) {
	this->begin = begin;
	this->end = end;
	std::ifstream stream(FILE);
	stream.seekg(begin);
	short pathLength;
	stream.read((char *)&pathLength, sizeof(pathLength));
	stream.read(reinterpret_cast<char*>(&lastEdited), sizeof(lastEdited));
}

File::~File() {}
