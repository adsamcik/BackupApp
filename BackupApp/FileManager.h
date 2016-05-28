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
	bool DeleteFile(const std::string&);
	void BackupFile(const std::string&);
	///Regenerates backup file with new reserves etc. Can be run if computer is idle and backup file is crowded.
	void RebuildBackups();
};

