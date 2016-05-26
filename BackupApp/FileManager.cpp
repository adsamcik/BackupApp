#include "stdafx.h"
#include "FileManager.h"



FileManager::FileManager() {
	stream = new std::ifstream(FILE);
}


FileManager::~FileManager() {}
