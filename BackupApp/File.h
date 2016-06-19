#pragma once

#include "dirent.h"

/**
FILE STRUCTURE
--Meta										STATIC_FILE_SIZE + string length
---PathLength (possibly faster loading)		4B (should every imaginable path, can't imagine 4gb large path)
---Path										XB
---Time										8B
---ContentEnd								sizeof(std::streamoff) most probably 8B == 0 if no content is saved
--Content

*/
#define BHEADER_SIZE sizeof(uint32_t) + sizeof(time_t) + 2* sizeof(std::streamoff)
#ifdef _WIN32
#define timegm _mkgmtime
#endif

#define BUFFER_SIZE 512
#define FILE_RESERVE 1024

class File {
private:
	/**
		Cached path to file, can be null. Use functions GetPath() and ClearPath()
	*/
	std::string* path;
public:
	/**
		First byte of file meta data
	*/
	std::streamoff beginMeta;

	/**
		First byte of content
	*/
	std::streamoff beginContent;

	/**
		First byte out of range of content = beginMeta of the next file
	*/
	std::streamoff endContent;

	/**
		Number of bytes in reserve
	*/
	std::streamoff reserve;

	/**
		Last edit of file (Can differ from live file)
	*/
	tm* lastEdited = nullptr;

	File(const std::string& path);
	File(std::fstream& stream);
	virtual ~File();

	/**
		Restores file to the original location
		@param stream stream to the backup file
	*/
	ext::Success Restore(std::fstream &stream) const;

	/**
		Writes metadata to stream
	*/
	void WriteMeta(std::fstream *stream);

	/**
		Checks if newer version of file is available
	*/
	bool IsNewer();

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

/**
	Has additional function and is used to identify directories.
*/
class Dir : public File {
public:
	Dir(const std::string& path);

	/**
		@return pointer to list of files in directory, excluding . and ..
	*/
	std::vector<std::string>* GetFiles();
};

