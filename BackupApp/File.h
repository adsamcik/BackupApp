#pragma once

class File { 
public:
	std::streamoff beginMeta;
	std::streamoff beginContent;
	std::streamoff endContent;

	tm* lastEdited;
	std::string path;

	File(std::fstream& stream, const std::streampos& beginMeta);
	~File();

	void Restore(std::fstream & stream);
};

