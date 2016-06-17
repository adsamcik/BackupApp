#include "stdafx.h"
#include "FileManager.h"
#include "stdlib.h"
#include "Config.h"
#include "Console.h"
#include <ctime>

using std::string;

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

bool FileManager::Open() {
	stream->open(BACKUP_FILE, std::fstream::in | std::fstream::out | std::fstream::binary);
	bool isOpen = stream->is_open();
	if (!isOpen)
		Console::PrintError("Failed to open backup file!");
	return isOpen;
}

void FileManager::Close() {
	stream->close();
}

int FileManager::CompareDates(const tm * td1, const tm * td2) const {
	tm* first = new tm(*td1);
	tm* second = new tm(*td2);
	auto diff = timegm(first) - timegm(second);
	return diff > 0 ? 1 : diff < 0 ? -1 : 0;
}

bool FileManager::IsEmpty() const {
	if (isEmpty)
		Console::PrintError("Nothing is backed up yet.");
	return isEmpty;
}

void FileManager::Clear() {
	stream->open(BACKUP_FILE, std::ios::trunc | std::ios::out);
	if (stream->is_open())
		std::cout << "Cleared all backups" << std::endl;
	else
		Console::PrintError("Failed to clear backups");
	Close();
}

void FileManager::Backup(File * file) {
	if (file->beginMeta >= 0)
		Backup(file, file->beginMeta);
	else
		Backup(file, fileEnd);
}

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

void FileManager::Remove(const std::string & path) {
	File* f;
	bool found = false;
	std::streampos beg;
	std::streampos pos;
	std::streampos end;
	do {
		pos = stream->tellg();
		f = new File(*stream);
		if (ext::startsWith(*f->GetPath(), path)) {
			if (!found) {
				beg = pos;
				found = true;
			}
		}
		else if (found) {
			found = false;
			OffsetData(pos, beg - pos);
		}
		end = f->endContent;
		delete f;
	} while (stream->seekg(end).peek() != EOF);
}

void FileManager::PickRestore(std::vector<File*>& files) const {
	if (files.size() == 0)
		Console::PrintError("No files found");
	else if (files.size() == 1)
		files[0]->Restore(*stream);
	else {
		Console c(2);
		for (int i = 0; i < files.size(); i++)
			c.Add(*files[i]->GetPath());
		c.Print(true);
	}
}

void FileManager::PrintContent(const int contentLimit) {
	if (!Open() || IsEmpty())
		return;
	File* f = nullptr;
	std::streampos beg;
	std::streamoff end;
	do {
		beg = stream->tellg();
		f = new File(*stream);
		try {
			auto path = f->GetPath();
			if (f->lastEdited->tm_hour == -1) {
				if (ext::isValidPath(path->c_str()))
					throw std::exception(("DATE FOR FILE \"" + *path + "\" IS CORRUPTED").c_str());
				else
					throw std::exception(("BACKUP FILE IS CORRUPTED. Detected at " + std::to_string(static_cast<std::streamoff>(beg))).c_str());
			}
			else if (f->endContent < 0)
				throw std::exception(("FILE " + *path + " has incorrect end content position").c_str());
			char* buffer = new char[80];
			auto ptr = strftime(buffer, 80, "%c", f->lastEdited);
			std::cout << std::endl << *path << std::endl
				<< buffer << std::endl
				<< "Begin position: " << f->beginMeta << std::endl
				<< "Begin content: " << f->beginContent << std::endl
				<< "Content length: " << f->endContent - f->beginContent << std::endl;
			delete[] buffer;
			stream->seekg(f->beginMeta + f->beginContent);
			if (contentLimit > 0) {
				int64_t toLoad = (contentLimit > f->endContent - f->beginContent) ? f->endContent - f->beginContent : contentLimit;
				buffer = new char[toLoad + 1];
				stream->read(buffer, toLoad);
				buffer[toLoad] = '\0';
				std::cout << buffer << std::endl;
				delete[] buffer;
				stream->seekg(-toLoad, std::ios::cur);
			}
		}
		catch (std::exception e) {
			Console::PrintError(string(e.what()));
			delete f;
			break;
		}
		beg = f->beginContent;
		end = f->endContent;
		delete f;
	} while (stream->seekg(end - beg, std::ios::cur).peek() != EOF);
	Close();
}

void FileManager::BackupAll() {
	if (!Open())
		return;
	auto paths = Config::paths;
	if (paths.size() > 0)
		isEmpty = false;
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

void FileManager::Backup(File *file, const std::streampos &beg) {
	if (file->beginMeta != -1) {
		struct stat s;
		auto r = stat(file->GetPath()->c_str(), &s);
		tm* t = new tm();
		gmtime_s(t, &s.st_mtime);
		int diff = CompareDates(file->lastEdited, t);
		if (diff != 1) {
			std::cout << file->GetPath() << " is up to date" << std::endl;
			file->ClearPath();
			return;
		}
	}
	std::ifstream ostream(*file->GetPath(), std::ifstream::ate | std::ifstream::binary);
	std::streamoff length = ostream.tellg();
	ostream.seekg(ostream.beg);

	file->beginMeta = beg;
	file->beginContent = BHEADER_SIZE + file->GetPath()->size();
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
	std::cout << "Backed up " << file->GetPath()->c_str() << std::endl;
}

void FileManager::Backup(Dir *dir) {
	auto v = dir->GetFiles();
	for (auto filename : *v) {
		auto fullname = *dir->GetPath() + '\\' + filename;
		if (ext::isValidPath(fullname.c_str())) {
			if (ext::isDir(fullname.c_str())) {
				auto d = new Dir(fullname);
				Backup(d);
				delete d;
			}
			else {
				File *f = nullptr;
				auto pos = stream->tellg();
				auto tf = new File(*stream);
				if (stream->peek() != EOF) {
					if (tf->GetPath()->length() == 0)
						Console::PrintError("File path is 0, is something wrong?");
					else {
						auto index = ext::fullPath(*tf->GetPath()).find_last_of("/\\");
						if (tf->GetPath()->substr(index) == filename && tf->GetPath()->substr(0, index) == *dir->GetPath())
							f = tf;
					}
				}
				if (f == nullptr)
					f = new File(fullname);
				stream->clear();
				Backup(f, pos);
				delete f;
				delete tf;
			}
		}
		else
			Console::PrintError(filename + " is invalid path");
	}
	delete v;
}