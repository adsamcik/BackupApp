#include "stdafx.h"
#include <sstream>
#include "FileManager.h"
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
	const ext::Success AddPath(const std::string& path);

	/**
		\return Success (Successfull when path is found)
	*/
	const ext::Success RemovePath(const std::string& path);
	const ext::Success RemovePath(const size_t& path);

	/**
		\return Success
	*/
	const ext::Success Save();

	void Edit(FileManager &fm);

	/**
		User input functions
		Start with U
	*/
	void USetDay(const std::string& line);
	void UList();
	void UAdd(FileManager &fm, const std::string& line);
	void URemove(FileManager &fm, const std::string& line);
	void URemove(FileManager &fm, const size_t& index);

	///paths to back up
	std::vector<std::string> paths;
private:
	///Auto backup enabled
	bool abEnabled;

	void SetDay(const int day);
	///Day on which files should be backed up
	ext::DayOfWeek day;


	void PrintOptions();
};

