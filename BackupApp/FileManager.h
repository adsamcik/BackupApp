#pragma once
#include <stdio.h>
#include <iostream>
#include "File.h"


class FileManager {
private:
	std::fstream* stream;
	std::streampos fileEnd;
	std::vector<File*> files;

	void Open();
	void Close();
	void WriteMeta(File *file);

public:
	FileManager();
	~FileManager();
	bool DeletePath(const std::string&);
	void AddPath(const std::string&);

	///Backs up files
	void Backup(File*);
	///Backs up all files and dirs in directory
	void Backup(Dir*);
	///Moves all data further in file to make space for file update that exceeds reserved file size
	/// @param beg beginning of offset
	/// @param off offset
	void OffsetData(const std::streampos &beg, const std::streamoff &off);
	///Regenerates backup file with new reserves etc. Can be run if computer is idle and backup file is crowded.
	void RebuildBackups();
	void BackupAll();
	void BackupAll(std::vector<std::string>& paths);
};

