#pragma once
#include <stdio.h>
#include <iostream>
#include "File.h"


class FileManager {
private:
	std::fstream* stream;
	std::streampos metaBegin;
	std::streampos fileEnd;

	///Backs up file on path
	void Backup(const std::string&);
	void WriteMeta();
	void WriteMeta(const File &file);

	std::vector<File*> files;
public:
	FileManager();
	~FileManager();
	bool DeletePath(const std::string&);

	///Backs up all files on path
	void BackupPath(File&);
	void OffsetData(const std::streampos &beg, const std::streamoff &off);
	///Regenerates backup file with new reserves etc. Can be run if computer is idle and backup file is crowded.
	void RebuildBackups();
	void BackupAll();
	void BackupAll(std::vector<std::string>& paths);
};

