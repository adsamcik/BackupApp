#pragma once
#include <stdio.h>
#include <iostream>


class FileManager {
private:
	std::ifstream* stream;
public:
	FileManager();
	~FileManager();
	bool DeletePath(const std::string&);
	void BackupPath(const std::string&);
	///Regenerates backup file with new reserves etc. Can be run if computer is idle and backup file is crowded.
	void RebuildBackups();
};

