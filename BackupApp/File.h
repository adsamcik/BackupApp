#pragma once

class File { 
private:
	std::streampos begin;
	std::streampos beginContent;
	std::streampos end;

	tm lastEdited;
	std::string path;

public:
	File(const std::streampos&, const std::streampos&);
	~File();

	void Restore();
};

