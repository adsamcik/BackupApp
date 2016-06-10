#pragma once

class File { 
private:
	std::streampos beginMeta;
	std::streampos beginContent;
	std::streampos endContent;

	tm* lastEdited;
	std::string path;

public:
	File(std::fstream& stream, const std::streampos& beginMeta);
	~File();

	void Restore(std::fstream & stream);
};

