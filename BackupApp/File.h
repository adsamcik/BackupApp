#pragma once
#include <iostream>
#include <fstream>

class File { 
private:
	std::streampos begin;
	std::streampos end;

	tm time;
	char* path;
public:
	File(const std::streampos&, const std::streampos&);
	~File();

	void Restore();
};

