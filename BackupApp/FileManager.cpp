#include "stdafx.h"
#include "FileManager.h"

/*	BACKUP FILE STRUCTURE
	8B - position of content
	xB - content (actual content of files)
	xB - meta-data (information about filenames, beginnings and ends of files)

	meta-data are placed at the end, because they will be most often rewritten and we don't have to rewrite the whole file when lots of new files are added.
*/

FileManager::FileManager() {
	stream = new std::ifstream(FILE);
}


FileManager::~FileManager() {}


