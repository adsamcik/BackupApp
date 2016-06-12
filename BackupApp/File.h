#pragma once
#ifdef _WIN32 
#include "dirent.h"
#elif __linux__
#include <dirent.h>
#endif

/*
FILE STRUCTURE
--Meta										12B + 2*sizeof(std::streamoff)B + sizeof(string)B
---PathLength (possibly faster loading)		4B (should every imaginable path, can't imagine 4gb large path)
---Path										XB
---Time										8B
---ContentBegin								sizeof(std::streamoff) most probably 8B == 0 if no content is saved
---ContentEnd								sizeof(std::streamoff) most probably 8B == 0 if no content is saved
--Content												

*/
class File { 
public:
	std::streamoff beginMeta = 0;
	std::streamoff beginContent = 0;
	std::streamoff endContent = 0;

	tm* lastEdited;

	File(const std::string& path);
	File(std::fstream& stream, const std::streampos& beginMeta);
	~File();

	virtual void Restore(std::fstream & stream) const;
	bool IsValid() const;

	std::string* GetPath();
	std::string GetPath() const;
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

