#include "stdafx.h"
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

	/**
	\return Success
	*/
	ext::Success Save();

	void Edit();
private:
	///paths to back up
	std::vector<std::string> paths;
	///Auto backup enabled
	bool abEnabled;
	///Day on which files should be backed up
	ext::DayOfWeek day;

	void PrintOptions();
};

