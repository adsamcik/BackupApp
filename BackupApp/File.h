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
private:
	///Original path
	std::string* path;
public:
	///Begin of meta
	std::streamoff beginMeta;
	///Begin of file
	std::streamoff beginContent;
	///End of file
	std::streamoff endContent;
	///End of file content + reserve
	std::streamoff endFile;
	///Last edited
	tm* lastEdited;

	File(const std::string& path);
	File(std::fstream& stream, const std::streampos& beginMeta);
	~File();

	/**
		Restores file to the original location
		@param stream stream to the backup file
	*/
	virtual void Restore(std::fstream &stream) const;

	void WriteMeta(std::fstream *stream);

	/**
		Checks whether the path still exists or not
	*/
	bool IsValid() const;

	/**
		Function returns path. Utilizes caching to path variable on File object.
		@return string with path to the file
	*/
	std::string* GetPath();

	/**
		Function returns path. Does not utilize caching to path variable on File object.
		@return string with path to the file
	*/
	char* GetPath() const;

	/**
		Clears path from memory if cached
	*/
	void ClearPath();
};

///Has additional function and is used to identify directories.
class Dir : public File {
public:	
	Dir(const std::string& path);

	void Restore(std::fstream & stream) const override;

	/**
		@return pointer to list of files in folder, excluding . and ..
	*/
	std::vector<std::string>* GetFiles();
};

