#include "stdafx.h"
#include "Config.h"


Config::Config() {
	std::ifstream stream(CONFIG);
	std::string line;
	while (std::getline(stream, line)) {
		std::istringstream iss(line);
		bool isDir;
		std::string path;
		if (!(iss >> isDir >> path)) { break; }
		AddPath(path);
	}
}

Config::~Config() {}

ext::Success Config::AddPath(const std::string & path) {
	return ext::Success();
}
