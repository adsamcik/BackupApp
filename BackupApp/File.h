#pragma once
#include <iostream>
#include <fstream>

class File { 
private:
	std::streampos begin;
	std::streampos beginContent;
	std::streampos end;

	tm time;
	std::string path;

public:
	File(const std::streampos&, const std::streampos&);
	~File();

	void Restore();
};

