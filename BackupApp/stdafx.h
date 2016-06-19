// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifdef _WIN32
#include "targetver.h"
#endif

#include <cstdio>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "Extensions.h"
#include <stdexcept>
#if defined _DEBUG && defined _WIN32
#include <vld.h>
#endif

#define BACKUP_FILE "backup.data"
#define CONFIG_FILE "backup.cfg"
