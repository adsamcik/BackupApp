#include "stdafx.h"
#include "FileManager.h"
#include "stdlib.h"
#include "Config.h"
#include "Console.h"

void FileManager::Open() {
	stream->open(BACKUP_FILE, std::fstream::in | std::fstream::out | std::fstream::binary);
}

void FileManager::Close() {
	stream->close();
}

FileManager::FileManager() {
	//fstream does not create file with fstream::in flag
	//this ensures the file exists
	if (!ext::isValidPath(BACKUP_FILE)) {
		std::ofstream outfile(BACKUP_FILE);
		outfile.close();
	}

	stream = new std::fstream(BACKUP_FILE, std::fstream::in | std::fstream::binary);
	stream->seekg(0, stream->end);
	fileEnd = stream->tellg();
	stream->seekg(0, stream->beg);
	/*auto beg = stream->tellg();
	if (fileEnd != stream->tellg()) {
		File *f;
		do {
			f = new File(*stream);
			files.push_back(f);
		} while (stream->seekg(f->endContent).peek() != EOF);
	}*/
	Close();

	//for (auto f : files)
	//	std::cout << "Loaded " << f->GetPath()->c_str() << std::endl;
}


FileManager::~FileManager() {
	stream->close();
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

void FileManager::Backup(File *file, const std::streampos &beg) {
	std::cout << "Backing up " << file->GetPath()->c_str() << std::endl;

	std::ifstream ostream(*file->GetPath(), std::ifstream::ate | std::ifstream::binary);
	std::streamoff length = ostream.tellg();
	ostream.seekg(ostream.beg);

	if (file->beginMeta == -1) {
		file->beginMeta = beg;
		file->beginContent = sizeof(uint32_t) + sizeof(time_t) + sizeof(file->endContent) + file->GetPath()->size();
		file->endContent = file->beginContent + length;
		stream->seekg(beg);
		file->WriteMeta(stream);
	}
	else
		stream->seekg(file->beginContent);


	if (file->endContent != -1 && length > file->endContent - file->beginContent) {
		auto off = static_cast<std::streamoff>(length * 1.1 - (file->endContent - file->beginContent));
		OffsetData(file->endContent, off);
	}

	*stream << ostream.rdbuf();

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
			Backup(f, stream->tellg());
			delete f;
		}

	}
	delete v;
}

/**
	Offset all data till the end of file
	Will cause terrible problems if beg is not at the beginning of a file!
*/
void FileManager::OffsetData(const std::streampos &beg, const std::streamoff &off) {
	//is signed to save on conversion when using in seek
	const int32_t bufferSize = 512;
	char *buffer = new char[bufferSize];
	stream->seekg(fileEnd);
	auto pos = fileEnd;
	stream->seekg(-bufferSize, std::ios::end);
	do {
		stream->seekg(-bufferSize, std::ios::cur);
		pos = stream->tellg();
		stream->read(buffer, bufferSize);
		stream->seekg(off - bufferSize, std::ios::cur);
		stream->write(buffer, bufferSize);
		stream->seekg(pos);
	} while (stream->tellg() - beg > bufferSize);
	delete[] buffer;

	auto diff = stream->tellg() - beg;
	if (diff > 0) {
		stream->seekg(beg);
		buffer = new char[diff];
		stream->read(buffer, diff);
		stream->seekg(off - diff, std::ios::cur);
		stream->write(buffer, diff);
		delete[] buffer;
	}
}

//Creates new backup file with restored reserves
void FileManager::RebuildBackups() {}

void FileManager::BackupAll() {
	auto paths = Config::paths;
	Open();
	/*for (auto file : paths) {
		if (!file->IsValid())
			Console::PrintError(*file->GetPath() + " is not a valid path!");
		else {
			//Checks whether file is directory or file
			auto r = dynamic_cast<Dir*>(file);
			if (r != nullptr)
				Backup(r);
			else
				Backup(file, fileEnd);
		}
		//file->ClearPath();
	}*/
	Close();
}