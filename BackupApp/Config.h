#include "stdafx.h"
#include <sstream>
#include "FileManager.h"
#pragma once

class Config {
public:
	/**
		Initializes configuration
	*/
	static void Initialize();

	/**
		Add path to config
		@return Success (Successfull when valid path and not already added)
	*/
	static const ext::Success AddPath(const std::string& path);

	/**
		Remove path from config
		@parameter path remove path by name
		@return Success
	*/
	static const ext::Success RemovePath(const std::string& path);

	/**
		Remove path from config
		@parameter index index of path to remove
		@return Success
	*/
	static const ext::Success RemovePath(const size_t& index);

	/**
		Returns day of the week on which the backup will occur
		@return day on which the backup will occur
	*/
	static ext::DayOfWeek GetDay();

	/**
		Function that handles user input
	*/
	static void Edit(FileManager &fm);

	/**
		User input function to set a day
		@param param parameters (without function call) 
	*/
	static ext::Success USetDay(const std::string& param);

	/**
		User input function
		Draws a list to cout of backed up paths
		@param param parameters (without function call)
	*/
	static void UList();

	/**
		User input function
		Checks if path is valid and if so adds it to backed up list
		@param param parameters (without function call)
	*/
	static void UAdd(const std::string& param);

	/**
		User input function
		Remove path by string
		@param param parameters (without function call)
	*/
	static void URemove(FileManager &fm, const std::string& param);

	/**
		User input function
		Remove path by index
		@param index index (can be found with list function)
	*/
	static void URemove(FileManager &fm, const size_t& index);

	/**
		Paths to back up
	*/
	static std::vector<std::string> paths;
private:

	/**
		@return Success true if successful
	*/
	static const ext::Success Save();

	/**
		Day on which files should be backed up
	*/
	static ext::DayOfWeek day;

	/**
		Prints help for Config to cout
	*/
	static void PrintOptions();
};

