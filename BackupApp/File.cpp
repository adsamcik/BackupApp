#include "stdafx.h"
#include "File.h"
#include <istream>
#include <fstream>

/*
	FILE STRUCTURE
	--Content
	////////stuff
	--Meta
	---PathLength (possibly faster loading)		4B (should every imaginable path, can't imagine 4gb large path)
	---Path										XB
	---Time										8B
	---ContentBegin								8B
	---ContentEnd								8B
*/


/*File::File(const std::streampos & beginMeta, const std::streampos & endContent, const std::string& path) {
	this->beginMeta = beginMeta;
	this->endContent = endContent;
	std::ifstream stream(BACKUP_FILE);
	stream.seekg(beginMeta);
	short pathLength;
	stream.read((char *)&pathLength, sizeof(pathLength));
	stream.read(reinterpret_cast<char*>(&lastEdited), sizeof(lastEdited));
}*/

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
	lastEdited = reinterpret_cast<tm>(mLong);
	stream.get(mLong, 8);
	stream.get(mLong, 8);

	delete[] mLong;
	delete[] mInt;
}

File::~File() {}
