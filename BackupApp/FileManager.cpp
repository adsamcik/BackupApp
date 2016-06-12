#include "stdafx.h"
#include "FileManager.h"
#include "stdlib.h"
#include "Config.h"
#include "Console.h"

void FileManager::WriteMeta(File *file) {
	auto path = file->GetPath();
	file->beginMeta = stream->tellg();
	*stream << path->size() << *path << file->lastEdited << file->beginContent << file->endContent;
}

FileManager::FileManager() {
	//fstream does not create file with fstream::in flag
	//this ensures the file exists
	std::ofstream outfile(BACKUP_FILE);
	outfile.close();

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
	for (auto file : files)
		delete file;
}

bool FileManager::DeletePath(const std::string &) {
	return false;
}

void FileManager::AddPath(const std::string &path) {
	if (ext::isDir(path.c_str()))
		files.push_back(new Dir(path));
	else
		files.push_back(new File(path));
}

void FileManager::Backup(File *file) {
	std::cout << "Backing up " << file->GetPath()->c_str() << std::endl;
	WriteMeta(file);

	if (file->beginContent == std::streampos(0))
		file->beginContent = fileEnd;

	std::ifstream ostream(*file->GetPath(), std::ios::binary);
	ostream.seekg(ostream.end);
	std::streamoff length = ostream.tellg();
	ostream.seekg(ostream.beg);

	if (file->endContent == std::streampos(0) || length > file->endContent - file->beginContent) {
		auto off = static_cast<std::streamoff>(length * 1.1 - (file->endContent - file->beginContent));
		OffsetData(file->endContent, off);
		file->endContent += off;
	}

	stream->seekg(file->beginContent);
	char buffer[100];
	while (ostream.read(buffer, sizeof(buffer)))
		*stream << buffer;

	auto pos = stream->tellg();
	if (pos > fileEnd)
		fileEnd = pos;
	//file->ClearPath();
}

void FileManager::Backup(Dir *dir) {
	auto v = dir->GetFiles();
	for (auto path : *v) {
		if (ext::isDir(path.c_str())) {
			auto d = new Dir(path);
			Backup(d);
			delete d;
		}
		else {
			auto f = new File(*dir->GetPath() + '/' + path);
			Backup(f);
			delete f;
		}

	}
	delete v;
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
}

//Creates new backup file with restored reserves
void FileManager::RebuildBackups() {}

void FileManager::BackupAll() {
	for (auto file : files) {
		if (!file->IsValid())
			Console::PrintError(*file->GetPath() + " is not a valid path!");
		else {
			//Checks whether file is directory or file
			auto r = dynamic_cast<Dir*>(file);
			if (r != nullptr)
				Backup(r);
			else
				Backup(file);
		}
		//file->ClearPath();
	}
	metaBegin = stream->tellg();
}