#include "stdafx.h"
#include <set>
#include <sstream>
#pragma once

class Config {
public:
	/**
		Loads config files on initialization
	*/
	Config();

	~Config();

	/**
		\return Success (Successfull when valid path and not already added)
	*/
	ext::Success AddPath(const std::string& path);

	/**
		\return Success (Successfull when path is found)
	*/
	ext::Success RemovePath(const std::string& path);
private:
	std::set<std::string> paths;
};

