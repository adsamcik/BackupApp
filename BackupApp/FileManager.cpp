#include "stdafx.h"
#include "FileManager.h"
#include "stdlib.h"
#include "Config.h"
#include "Console.h"
#include <ctime>
#ifdef __linux
#include <unistd.h>
#endif
#ifdef _WIN32
#include <io.h>
#endif

#define DBG
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
	if (!isOpen) {
		if (!ext::isValidPath(BACKUP_FILE)) {
			std::ofstream outfile(BACKUP_FILE);
			if (!outfile.is_open()) {
				Console::PrintError("Failed to recreate backup file!");
				return false;
			}
			outfile.close();
			isEmpty = true;
			Console::PrintWarning("Please do not delete backup file while the app is running. File recreated.");
			return Open();
		}
		else
			Console::PrintError("Failed to open backup file!");
	}
	return isOpen;
}

void FileManager::Close() {
	stream->close();
}

bool FileManager::IsEmpty() const {
	if (isEmpty)
		Console::PrintError("Nothing is backed up yet.");
	return isEmpty;
}

void FileManager::Clear() {
	stream->open(BACKUP_FILE, std::ios::trunc | std::ios::out);
	if (stream->is_open()) {
		std::cout << "Cleared all backups" << std::endl;
		isEmpty = true;
		fileEnd = 0;
		Close();
	}
	else
		Console::PrintError("Failed to clear backups");
}

void FileManager::OffsetForward(const std::streampos & beg, const std::streamoff & off, const int32_t bufferSize) {
	char *buffer = new char[bufferSize];
	auto pos = beg;
	do {
		stream->seekg(pos);
		stream->read(buffer, bufferSize);
		stream->seekg(-bufferSize + off, std::ios::cur);
		stream->write(buffer, bufferSize);
		pos += bufferSize;
	} while (fileEnd - pos > bufferSize);
	delete[] buffer;

	auto diff = fileEnd - pos;
	if (diff > 0) {
		stream->seekg(pos);
		buffer = new char[diff];
		stream->read(buffer, diff);
		stream->seekg(-bufferSize + off, std::ios::cur);
		stream->write(buffer, diff);
		delete[] buffer;
	}
	std::cout << fileEnd << " / " << off << std::endl;
	Close();
	Truncate(static_cast<off_t>(off));
	Open();
	fileEnd += off;
	if (fileEnd == std::streampos(0))
		isEmpty = true;
}

void FileManager::OffsetBackward(const std::streampos & beg, const std::streamoff & off, const int32_t bufferSize) {
	char *buffer;
	stream->seekg(0, std::ios::end);
	//ensures seekg doesn't fail
	stream->write(string(off, '\0').c_str(), off);
	stream->seekg(fileEnd);
	auto pos = fileEnd;
	if (pos - beg >= bufferSize) {
		buffer = new char[bufferSize];
		//stream->seekg(-bufferSize, std::ios::end);
		do {
			stream->seekg(-bufferSize, std::ios::cur);
			pos = stream->tellg();
			stream->read(buffer, bufferSize);
			stream->seekg(off - bufferSize, std::ios::cur);
			stream->write(buffer, bufferSize);
			stream->seekg(pos);
		} while (stream->tellg() - beg > bufferSize);
		delete[] buffer;
	}

	auto diff = stream->tellg() - beg;
	if (diff > 0) {
		stream->seekg(beg);
		buffer = new char[diff];
		stream->read(buffer, diff);
		stream->seekg(off - diff, std::ios::cur);
		stream->write(buffer, diff);
		delete[] buffer;
	}
	fileEnd += off;
}


void FileManager::Offset(const std::streampos &beg, const std::streamoff &off) {
	/*if (fileEnd == std::streampos(0)) {
		Console::PrintError("File size is 0");
		return;
	}*/
	std::cout << "Offsetting data by " << off << std::endl;
	if (off > 0)
		OffsetBackward(beg, off, BUFFER_SIZE);
	else if (off < 0)
		OffsetForward(beg, off, BUFFER_SIZE);
	else
		Console::PrintError("Offset is set to 0, this can't be right.");
}

void FileManager::Truncate(const off_t shrinkBy) const {
#ifdef __linux
	truncate(BACKUP_FILE, static_cast<off_t>(fileEnd) + shrinkBy);
#else
	truncate(BACKUP_FILE, shrinkBy);
#endif
}


#ifdef _WIN32
int FileManager::truncate(const char *path, const off_t shrinkBy) const {
	string fullPath = ext::fullPath(path);
	wchar_t *wtext = new wchar_t[fullPath.length() + 1];
	mbstowcs(wtext, fullPath.c_str(), fullPath.length() + 1);//Plus null
	LPWSTR ptr = wtext;

	HANDLE hFile = CreateFile(ptr, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, 0);
	SetFilePointer(hFile, shrinkBy, 0, FILE_END);
	SetEndOfFile(hFile);
	CloseHandle(hFile);
	delete[] wtext;
	return 0;
}
#endif

//Creates new backup file with restored reserves
void FileManager::RebuildBackups() {}

void FileManager::Remove(const std::string & path) {
	if (!Open() || isEmpty)
		return;
	File* f;
	bool found = false;
	std::streampos beg;
	std::streampos pos;
	std::streampos end;

	stream->seekg(0);
	do {
		pos = stream->tellg();
		f = new File(*stream);
		if (ext::startsWith(*f->GetPath(), path)) {
			std::cout << "Found " << *f->GetPath() << std::endl;
			if (!found) {
				beg = pos;
				found = true;
			}
		}
		else if (found) {
			found = false;
			Offset(pos, beg - pos);
		}
		end = f->beginMeta + f->endContent;
		delete f;
	} while (stream->seekg(end).peek() != EOF);

	if (found)
		Offset(fileEnd, beg - fileEnd);

	Close();
}

void FileManager::Restore(const std::string &name, const bool dir) {
	if (!Open() || IsEmpty())
		return;

	string path = dir ? ext::fullPath(name) : name;
#ifdef _WIN32
	if (dir)
		path[0] = std::toupper(path[0]);
#endif

	File *f = nullptr;
	std::streamoff end;
	std::vector<File*> files;
	stream->clear();
	stream->seekg(0);

	if (dir)
		std::cout << "Converted to full path " << path << std::endl;
	do {
		try {
			f = new File(*stream);
		}
		catch (std::bad_alloc e) {
			Console::PrintError("Failed to allocate file... Weird..");
			return;
		}
		catch (std::exception e) {
			Console::PrintError(string(e.what()) + "   Aborting");
			delete f;
			return;
		}

		end = f->beginMeta + f->endContent;

		try {
			f->GetPath();
		}
		catch (std::exception e) {
			std::cout << "Backup file is corrupted!" << std::endl;
			delete f;
			return;
		}

		if (dir) {
			if (ext::startsWith(*f->GetPath(), path))
				files.push_back(f);
			else
				delete f;
		}
		else if (*f->GetPath() == path) {
			f->Restore(*stream);
			Close();
			delete f;
			return;
		}
		else if (f->GetPath()->find(path) != string::npos)
			files.push_back(f);
		else
			delete f;
	} while (stream->seekg(end).peek() != EOF);

	if (files.size() == 0)
		Console::PrintWarning("No files found " + path);
	else {
		if (dir) {
			string parent;
			for (auto file : files) {
				string tp = ext::parent(*file->GetPath());
				if (parent != tp) {
#ifdef _WIN32
					system(("mkdir " + tp).c_str());
#else 
					system(("mkdir -p " + tp).c_str());
#endif
					parent = tp;
				}
				file->Restore(*stream);
				delete file;
			}
		}
		else
			PickRestore(files);
	}
	Close();
}

void FileManager::PickRestore(std::vector<File*>& files) const {
	if (files.size() == 0)
		Console::PrintError("No files found");
	else {
		//Console::Clear();
		std::cout << "Found these matches. Write index of the one you want to restore." << std::endl << "Invalid index or text will abort the restore." << std::endl;
		Console c(1);
		for (size_t i = 0; i < files.size(); i++)
			c.Add(*files[i]->GetPath());
		c.Print(true);
		string in;
		getline(std::cin, in);
		bool restored = false;
		if (ext::isDigit(in)) {
			int val = std::stoi(in.c_str());
			if (val >= 0 && val < static_cast<int>(files.size())) {
				auto suc = files[val]->Restore(*stream);
				if (!suc)
					Console::PrintError(*suc.message);
				else
					restored = true;
			}
		}

		if (!restored)
			std::cout << "Aborting..." << std::endl;
		for (auto f : files)
			delete f;
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
		try {
			f = new File(*stream);
		}
		catch (std::exception e) {
			Console::PrintError(e.what());
			break;
		}
		try {
			auto path = f->GetPath();
			if (f->lastEdited->tm_hour == -1) {
				if (ext::isValidPath(path->c_str()))
					throw std::runtime_error(("DATE FOR FILE \"" + *path + "\" IS CORRUPTED").c_str());
				else
					throw std::runtime_error(("BACKUP FILE IS CORRUPTED. Detected at " + std::to_string(static_cast<std::streamoff>(beg))).c_str());
			}
			else if (f->endContent < 0)
				throw std::runtime_error(("FILE " + *path + " has incorrect end content position").c_str());
			char* buffer = new char[80];
			strftime(buffer, 80, "%c", f->lastEdited);
			string newer = f->IsNewer() ? "true" : "false";
			Console c(2);
			c.Add("Path:\t" + *path)
				.Add("Last edited:\t" + string(buffer))
				.Add("Begin position:\t" + std::to_string(f->beginMeta))
				.Add("Begin content:\t" + std::to_string(f->beginContent))
				.Add("Reserve:\t" + std::to_string(f->reserve))
				.Add("Is newer version available:\t" + newer)
				.Add("Content length:\t" + std::to_string(f->endContent - f->beginContent - f->reserve))
				.Print(false);
			delete[] buffer;
			stream->seekg(f->beginMeta + f->beginContent);
			if (contentLimit > 0) {
				int64_t toLoad = (contentLimit > f->endContent - f->beginContent) ? f->endContent - f->beginContent : contentLimit;
				buffer = new char[toLoad + 1];
				stream->read(buffer, toLoad);
				buffer[toLoad] = '\0';
				std::cout << "First " << toLoad << " symbols" << std::endl << buffer << std::endl;
				delete[] buffer;
				stream->seekg(-toLoad, std::ios::cur);
			}
			std::cout << std::endl;
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

File* FileManager::GetFileFromStream(const string path)const {
	if (stream->is_open())
		stream->clear();
	else {
		Console::PrintError("Stream is closed!");
		return new File(path);
	}

	File *ts = nullptr;
	std::streampos end, beg;
	do {
		beg = stream->tellg();
		try {
			ts = new File(*stream);
		}
		catch (std::bad_alloc e) {
			Console::PrintError(e.what());
			break;
		}
		//does not throw error because it includes perfectly valid fail reasons, like eof
		catch (std::exception e) {
			if (ts != nullptr)
				delete ts;
			break;
		}

		try {
			ts->GetPath();
		}
		catch (std::exception e) {
			Console::PrintError(e.what());
			delete ts;
			break;
		}

		if (ts->GetPath()->length() == 0) {
			Console::PrintError("File path length is 0");
			delete ts;
			break;
		}
		else {
			if (ext::startsWith(*ts->GetPath(), ext::parent(path))) {
				int cmp = strcmp(ts->GetPath()->c_str(), path.c_str());
				if (cmp == 0) {
					stream->seekg(ts->beginMeta);
					return ts;
				}
				else if (cmp > 0) {
					delete ts;
					break;
				}
			}
			else {
				delete ts;
				break;
			}
		}
		end = ts->beginMeta + ts->endContent;
		delete ts;
		ts = nullptr;
	} while (stream->seekg(end).peek());
	stream->seekg(beg);
	return new File(path);
}

void FileManager::Backup(File * file) {
	if (stream->is_open())
		stream->clear();
	else {
		Console::PrintError("Stream is closed!");
		return;
	}
	if (file->beginMeta >= 0)
		Backup(file, file->beginMeta);
	else
		Backup(file, stream->tellg());
}

void FileManager::Backup(File *file, const std::streampos &beg) {
	if (stream->is_open())
		stream->clear();
	else {
		Console::PrintError("Stream is closed!");
		return;
	}

	if (file->beginMeta != -1) {
		if (!file->IsNewer()) {
			std::cout << *file->GetPath() << " is up to date" << std::endl;
			file->ClearPath();
			return;
		}
	}
	else {
		file->reserve = FILE_RESERVE;
	}
	std::ifstream ostream(*file->GetPath(), std::ifstream::ate | std::ifstream::binary);
	if (ostream.fail()) {
		Console::PrintError("Failed to open " + *file->GetPath() + "  ...  Skipping");
		return;
	}
	std::streamoff length = ostream.tellg();
	ostream.seekg(ostream.beg);

	auto clength = file->endContent - file->beginContent - file->reserve;
	if (file->endContent != -1 && length != clength) {
		if (length - clength > file->reserve) {
			Offset(file->beginMeta + file->endContent, length - clength + (FILE_RESERVE - file->reserve));
			file->reserve = FILE_RESERVE;
		}
		else {
			file->reserve -= length - clength;
		}
	}

	file->beginContent = BHEADER_SIZE + file->GetPath()->size();
	file->endContent = file->beginContent + length + file->reserve;
	if (file->beginMeta == -1 && beg != fileEnd)
		Offset(beg, file->endContent);
	file->beginMeta = beg;
	stream->clear();
	stream->seekg(beg);
	file->WriteMeta(stream);

	if (length > 0)
		*stream << ostream.rdbuf();
	stream->write(string(file->reserve, '\0').c_str(), file->reserve);

	auto pos = stream->tellg();
	if (pos > fileEnd)
		fileEnd = pos;
#ifdef DBG
	std::cout << "Backed up " << file->GetPath()->c_str() << std::endl;
#endif
}

void FileManager::Backup(Dir *dir) {
	auto v = dir->GetFiles();
	std::cout << "Backing up dir " << *dir->GetPath() << std::endl;
	for (auto filename : *v) {
		auto fullname = *dir->GetPath() + SEPARATOR + filename;
		if (ext::isValidPath(fullname.c_str())) {
			if (ext::isDir(fullname.c_str())) {
				auto d = new Dir(fullname);
				Backup(d);
				delete d;
			}
			else {
				auto f = GetFileFromStream(fullname);
				Backup(f);
				stream->seekg(f->beginMeta + f->endContent);
				delete f;
			}
		}
		else
			Console::PrintError(fullname + " is invalid path");
	}
	delete v;
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
			//Console c(1);
			//c.Add(*d->GetFiles()).Print(false);
			Backup(d);
			delete d;
		}
		else if (ext::isValidPath(path)) {
			File *f = GetFileFromStream(path);
			Backup(f);
			stream->seekg(f->beginMeta + f->endContent);
			delete f;
		}
		else
			Console::PrintError(path + " is not a valid path!");
	}
	Close();
}