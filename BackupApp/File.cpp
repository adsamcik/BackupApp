#include "stdafx.h"
#include "File.h"
#include <istream>
#include <fstream>
#include <ctime>

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

File::~File() {}

void File::Restore(std::fstream& stream) {
	std::ofstream outfile;
	outfile.open(path);
	std::copy(beginContent, endContent, outfile.beg());
	outfile.close();
}
