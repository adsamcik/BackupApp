#pragma once
#include <stdio.h>
#include <iostream>
#include "File.h"

class FileManager {
private:
	bool isEmpty;
	std::fstream* stream;
	std::streampos fileEnd;

	bool Open();
	void Close();

	void PickRestore(std::vector<File*>& files) const;

	/**
	Handles offset forward
	*/
	void OffsetForward(const std::streampos &beg, const std::streamoff &off, const int32_t bufferSize);

	/**
	Handles offset backward
	*/
	void OffsetBackward(const std::streampos &beg, const std::streamoff &off, const int32_t bufferSize);

	void Truncate(const off_t length) const;
#ifdef  _WIN32
	int truncate(const char *path, const off_t shrinkBy) const;
#endif //  _WIN32
public:
	FileManager();
	~FileManager();

	/**
		Checks if file is empty, if so prints error
	*/
	bool IsEmpty() const;

	void Clear();

	/**
		Backs up file
	*/
	void Backup(File *file);

	/**
		Backs up file at given position
	*/
	void Backup(File *file, const std::streampos& beg);

	/**
		Backs up all files and dirs in directory
	*/
	void Backup(Dir* dir);

	/**
		Backs up all the files
		Uses incremental backup
	*/
	void BackupAll();

	/**
		Moves all data further in file to make space for file update that exceeds reserved file size
		@param beg beginning of the first file to offset
		@param off offset
	*/
	void Offset(const std::streampos &beg, const std::streamoff &off);

	/**
		Regenerates backup file with proper reserves
		And cleares unecessary empty space
	*/
	void RebuildBackups();

	/**
		Restores given file
		If more than one file matches the name a selection with indexes is given
	*/
	void Restore(const std::string& name) const;

	/**
		Removes all files on this path
	*/
	void Remove(const std::string& path);

	/**
		Debug function used to print data about backed up files
	*/
	void PrintContent(const int contentLimit);
};

