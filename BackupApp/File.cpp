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
	---ContentBegin								sizeof(std::streamoff) most probably 8B
	---ContentEnd								sizeof(std::streamoff) most probably 8B
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
