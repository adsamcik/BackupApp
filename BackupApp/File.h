#pragma once
#ifdef _WIN32 
#include "dirent.h"
#elif __linux__
#include <dirent.h>
#endif

/**
FILE STRUCTURE
--Meta										12B + sizeof(std::streamoff)B + sizeof(string)B
---PathLength (possibly faster loading)		4B (should every imaginable path, can't imagine 4gb large path)
---Path										XB
---Time										8B
---ContentEnd								sizeof(std::streamoff) most probably 8B == 0 if no content is saved
--Content												

*/
class File { 
public:
	std::streamoff beginMeta;
	std::streamoff beginContent;
	std::streamoff endContent;

	tm* lastEdited;

	File(const std::string& path);
	File(std::fstream& stream, const std::streampos& beginMeta);
	~File();

	virtual void Restore(std::fstream & stream) const;
	bool IsValid() const;

	///Returns path and saves it to path variable
	std::string* GetPath();
	///Returns path
	std::string GetPath() const;

	///Allows removing path from memory
	void ClearPath();
private:
	std::string* path;
};

class Dir : public File {
public:	
	uint32_t filesCount;

	Dir(const std::string& path);

	void Restore(std::fstream & stream) const override;
	std::vector<std::string>* GetFiles();
};

