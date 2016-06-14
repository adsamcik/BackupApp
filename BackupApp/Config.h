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
		@return Success (Successfull when valid path and not already added)
	*/
	const ext::Success AddPath(const std::string& path);

	/**
		@return Success (Successfull when path is found)
	*/
	const ext::Success RemovePath(const std::string& path);
	const ext::Success RemovePath(const size_t& path);

	/**
		@return Success
	*/
	const ext::Success Save();

	void Edit(FileManager &fm);

	/**
		User input function to set a day
		@param param parameters (without function call) 
	*/
	void USetDay(const std::string& param);

	/**
		User input function
		Draws a list to cout of backed up paths
		@param param parameters (without function call)
	*/
	void UList();

	/**
		User input function
		Checks if path is valid and if so adds it to backed up list
		@param param parameters (without function call)
	*/
	void UAdd(FileManager &fm, const std::string& param);

	/**
		User input function
		Remove path by string
		@param param parameters (without function call)
	*/
	void URemove(FileManager &fm, const std::string& param);

	/**
		User input function
		Remove path by index
		@param index index (can be found with list function)
	*/
	void URemove(FileManager &fm, const size_t& index);

	///paths to back up
	std::vector<std::string> paths;
private:
	///Auto backup enabled
	bool abEnabled;

	void SetDay(const int day);
	///Day on which files should be backed up
	ext::DayOfWeek day;

	/**
		Prints help for Config to cout
	*/
	void PrintOptions();
};

