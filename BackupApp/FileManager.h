#pragma once
#include <stdio.h>
#include <iostream>


class FileManager {
private:
	std::fstream* stream;
	std::streampos metaBegin;
	std::streampos fileEnd;
public:
	FileManager();
	~FileManager();
	bool DeletePath(const std::string&);
	void BackupPath(const std::string&);
	void BackupPath(const std::string&, const std::streampos pos);
	///Regenerates backup file with new reserves etc. Can be run if computer is idle and backup file is crowded.
	void RebuildBackups();
	void BackupAll();
	void BackupAll(std::vector<std::string>& paths);
};

