#pragma once
#include <stdio.h>
#include <iostream>
#define FILE ".BACKP.cache"


class FileManager {
private:
	std::ifstream* stream;
public:
	FileManager();
	~FileManager();
};

