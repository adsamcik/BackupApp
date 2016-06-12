#include "stdafx.h"
#include "FileManager.h"
#include "stdlib.h"
#include "Config.h"
#include "Console.h"

/*	BACKUP FILE STRUCTURE
	8B - position of meta-data
	xB - content (actual content of files)
	xB - meta-data (information about filenames, beginnings and ends of files)

	meta-data are placed at the end, because they will be most often rewritten and we don't have to rewrite the whole file when lots of new files are added.
*/

void FileManager::WriteMeta() {
	stream->seekg(metaBegin);
	for (auto file : files) {
		file->beginMeta = stream->tellg();
		WriteMeta(*file);
	}
}

void FileManager::WriteMeta(const File & file) {
	*stream << file.path.size() << file.path << file.lastEdited << file.beginContent << file.endContent;
}

FileManager::FileManager() {
	stream = new std::fstream(BACKUP_FILE, std::fstream::in | std::fstream::out | std::fstream::binary);
	stream->seekg(0, stream->end);
	fileEnd = stream->tellg();
	stream->seekg(0, stream->beg);
	if (fileEnd != stream->tellg()) {
		char* mPos = new char[8];
		stream->read(mPos, 8);
		metaBegin = reinterpret_cast<long long>(mPos);

		stream->seekg(metaBegin);
		std::streampos pos = metaBegin;
		while (!stream->eof())
			files.push_back(new File(*stream, pos));
	}
}


FileManager::~FileManager() {
	delete stream;
}

bool FileManager::DeletePath(const std::string &) {
	return false;
}

void FileManager::Backup(File *file) {
	if (file->beginContent == std::streampos(0))
		file->beginContent = metaBegin;

	std::ifstream ostream(file->path);
	ostream.seekg(ostream.end);
	std::streamoff length = ostream.tellg();
	ostream.seekg(ostream.beg);

	if (file->endContent == std::streampos(0) || length > file->endContent - file->beginContent) {
		auto off = static_cast<std::streamoff>(length * 1.1 - (file->endContent - file->beginContent));
		OffsetData(file->endContent, off);
		file->endContent += off;
	}

	stream->seekg(file->beginContent);
	*stream << ostream.rdbuf();
}

void FileManager::Backup(Dir *dir) {
	auto v = dir->GetFiles();
	for (auto path : *v) {
		if (ext::isDir(path.c_str())) {
			auto d = new Dir(path);
			Backup(d);
			delete d;
		}
	}
}

void FileManager::OffsetData(const std::streampos &beg, const std::streamoff &off) {
	long long length = fileEnd;
	stream->seekg(metaBegin);
	for (auto file : files) {
		if (file->beginContent > beg) {
			file->beginContent += off;
			file->endContent += off;
		}
	}
	WriteMeta();
}

void FileManager::RebuildBackups() {}

void FileManager::BackupAll() {
	for (auto file : files) {
		if (!file->IsValid())
			Console::PrintError(file->path + " is not a valid path!");
		else {
			//Checks whether file is directory or file
			auto r = dynamic_cast<Dir*>(file);
			if (r != nullptr)
				Backup(r);
			else
				Backup(file);
		}
	}
}