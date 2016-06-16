#include "stdafx.h"
#include "FileManager.h"
#include "stdlib.h"
#include "Config.h"
#include "Console.h"

using std::string;

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
		isEmpty = true;
	}

	stream = new std::fstream(BACKUP_FILE, std::fstream::in | std::fstream::binary);
	auto beg = stream->tellg();
	stream->seekg(0, stream->end);
	fileEnd = stream->tellg();
	isEmpty = beg == fileEnd;
	Close();
}


FileManager::~FileManager() {
	stream->close();
	delete stream;
}

bool FileManager::IsEmpty() const {
	if (isEmpty)
		Console::PrintError("Nothing is backed up yet.");
	return isEmpty;
}

void FileManager::Backup(File * file) {
	if (file->beginMeta >= 0)
		Backup(file, file->beginMeta);
	else
		Backup(file, fileEnd);
}

void FileManager::Backup(File *file, const std::streampos &beg) {
	std::cout << "Backing up " << file->GetPath()->c_str() << std::endl;

	std::ifstream ostream(*file->GetPath(), std::ifstream::ate | std::ifstream::binary);
	std::streamoff length = ostream.tellg();
	ostream.seekg(ostream.beg);

	file->beginMeta = beg;
	file->beginContent = sizeof(uint32_t) + sizeof(time_t) + sizeof(file->endContent) + file->GetPath()->size();
	file->endContent = file->beginContent + length;
	stream->seekg(beg);
	file->WriteMeta(stream);

	if (file->endContent != -1 && length > file->endContent - file->beginContent) {
		auto off = static_cast<std::streamoff>(length * 1.1 - (file->endContent - file->beginContent));
		OffsetData(file->endContent, off);
	}

	*stream << ostream.rdbuf();

	auto pos = stream->tellg();
	if (pos > fileEnd)
		fileEnd = pos;
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

void FileManager::Restore(const std::string &name) const {
	if (IsEmpty())
		return;
	File *f;
	std::streamoff end;
	std::vector<File*> files;
	do {
		f = new File(*stream);
		end = f->endContent;
		if (f->GetPath()->find(name) != string::npos)
			files.push_back(f);
		else
			delete f;
	} while (stream->seekg(end).peek() != EOF);
	PickRestore(files);
}


void FileManager::PickRestore(std::vector<File*>& files) const {
	if (files.size() == 0)
		Console::PrintError("No files found");
	else if (files.size() == 1)
		files[0]->Restore(*stream);
	else {
		Console c(2);
		for (int i = 0; i < files.size(); i++)
			c.AddLine(std::to_string(i) + '\t' + *files[i]->GetPath());
		c.Print();
	}
}

void FileManager::BackupAll() {
	auto paths = Config::paths;
	Open();
	for (auto path : paths) {
		if (ext::isDir(path.c_str())) {
			Dir *d = new Dir(path);
			Backup(d);
			delete d;
		}
		else if (ext::isValidPath(path)) {
			File *f = new File(path);
			Backup(f);
			delete f;
		}
		else
			Console::PrintError(path + " is not a valid path!");
	}
	Close();
}