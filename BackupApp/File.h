#pragma once
#ifdef _WIN32 
#include "dirent.h"
#elif __linux__
#include <dirent.h>
#endif

/*
FILE STRUCTURE
--Content
////////stuff
--Meta
---PathLength (possibly faster loading)		4B (should every imaginable path, can't imagine 4gb large path)
---Path										XB
---Time										8B
---ContentBegin								sizeof(std::streamoff) most probably 8B
---ContentEnd								sizeof(std::streamoff) most probably 8B
------------Dir only-----------------
---File count								4B (next x files will belong to this folder)
*/
class File { 
public:
	std::streamoff beginMeta;
	std::streamoff beginContent;
	std::streamoff endContent;

	tm* lastEdited;
	std::string path;

	File(const std::string& path);
	File(std::fstream& stream, const std::streampos& beginMeta);
	~File();

	virtual void Restore(std::fstream & stream);
	bool IsValid();
};

class Dir : public File {
public:	
	uint32_t filesCount;

	Dir(const std::string& path);

	void Restore(std::fstream & stream) override;
	std::vector<std::string>* GetFiles();
};

