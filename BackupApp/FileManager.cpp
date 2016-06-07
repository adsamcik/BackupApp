#include "stdafx.h"
#include "FileManager.h"
#include "stdlib.h"
#include "Config.h"
#ifdef _WIN32 
#include "dirent.h"
#elif __linux__
#include <dirent.h>
#endif

/*	BACKUP FILE STRUCTURE
	8B - position of meta-data
	xB - content (actual content of files)
	xB - meta-data (information about filenames, beginnings and ends of files)

	meta-data are placed at the end, because they will be most often rewritten and we don't have to rewrite the whole file when lots of new files are added.
*/

void FileManager::Backup(const std::string &) {
	stream->seekg(metaBegin);
}

void FileManager::WriteMeta() {
	stream->seekg(metaBegin);
}

FileManager::FileManager() {
	stream = new std::fstream(BACKUP_FILE);
	stream->seekg(0, stream->end);
	fileEnd = stream->tellg();
	stream->seekg(0, stream->beg);
	if (fileEnd != stream->tellg()) {
		char* pos = new char[8];
		stream->read(pos, 8);
		metaBegin = reinterpret_cast<long long>(pos);

		stream->seekg(metaBegin);
		while (true) {
			char c;
			auto r = stream->get(c);
		}
	}
}


FileManager::~FileManager() {}

bool FileManager::DeletePath(const std::string &) {
	return false;
}

void FileManager::BackupPath(const std::string &) {

}

void FileManager::BackupPath(const std::string &, const std::streampos pos) {

}

void FileManager::RebuildBackups() {}

void FileManager::BackupAll() {
	Config c;
	BackupAll(c.paths);
}

void FileManager::BackupAll(std::vector<std::string>& paths) {
	DIR *dpdf;
	struct dirent *epdf;

	dpdf = opendir("./");
	if (dpdf != NULL) {
		while (epdf = readdir(dpdf)) {
			printf("Filename: %s", epdf->d_name);
			// std::cout << epdf->d_name << std::endl;
		}
	}
	closedir(dpdf);
}