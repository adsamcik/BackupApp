#include "stdafx.h"
#include <cctype>
#include <algorithm>
#pragma once

#define ws " \t\n\r\f\v"

//Namespace ext is generally list of short functions that are used all around the app
//Functions shorter or equal to 3 lines are inline
//because it's still easy to read and it's quite short anyway
namespace ext {
	enum DayOfWeek {
		Undefined,
		Monday,
		Tuesday,
		Wednesday,
		Thursday,
		Friday,
		Saturday,
		Sunday
	};
	
	/**
		Converts all chars in string to lower case
		@param s string to convert
	*/
	static inline std::string& tolower(std::string& s) {
		for (size_t i = 0; i < s.length(); i++)
			s[i] = std::tolower(s[i]);
		return s;
	}

	/**
		Trims right side of string from white characters by default
		@param s string to trim from the right
		@param t white characters which should be trimmed (default " \t\n\r\f\v")
	*/
	static inline std::string& rtrim(std::string& s, const char* t = ws) {
		return s.erase(s.find_last_not_of(t) + 1);;
	}

	/**
	Trims left side of string from white characters by default
	@param s string to trim from the left
	@param t white characters which should be trimmed (default " \t\n\r\f\v")
	*/
	static inline std::string& ltrim(std::string& s, const char* t = ws) {
		return s.erase(0, s.find_first_not_of(t));
	}

	/**
	Trims white characters on both sides of the string
	@param s string to trim
	@param t white characters which should be trimmed (default " \t\n\r\f\v")
	*/
	static inline std::string& trim(std::string& s, const char* t = ws) {
		return ltrim(rtrim(s, t), t);
	}

	/**
	Checks if string starts with another string
	@param string string to check
	@param what what should the string start with
	*/
	static bool startsWith(const std::string& string, const std::string& what) {
		if (string.length() < what.length())
			return false;
		for (size_t i = 0; i < what.length(); i++) {
			if (string[i] != what[i])
				return false;
		}
		return true;
	}

	/**
	Checks if all characters in string are digits
	@param str string to check
	*/
	static inline bool isDigit(const std::string &str) {
		return std::all_of(str.begin(), str.end(), ::isdigit);
	}

	/**
	Checks if given path is directory
	@param path path to check
	@return true if dir false if not dir or does not exists
	*/
	static inline bool isDir(const char* path) {
		struct stat path_stat;
		stat(path, &path_stat);
		return (path_stat.st_mode & S_IFDIR) == S_IFDIR;
	}

	/**
	Checks if given path is valid
	@param path path to check
	*/
	static inline bool isValidPath(const char* path) {
		struct stat info;
		return stat(path, &info) == 0;
	}

	/**
	Checks if given path is directory
	@param path path to check (converted to char*)
	@return true if dir false if not dir or does not exists
	*/
	static inline bool isValidPath(const std::string& path) {
		return isValidPath(path.c_str());
	}

	/**
	Compares 2 paths
	Accurate check should be ran on linux
	Contains only simple check on Windows due to requirement of rather bigger lib to ensure crossplatform compatibility
	*/
	static bool ComparePaths(const std::string &s1, const std::string &s2) {
#ifdef _WIN32 
		return s1 == s2;
#elif __linux__
		char* r1, r2;
		realpath(s1.c_str(), r1);
		realpath(s2.c_str(), r2);
		return strcmp(r1, r2) == 0;
#endif
	}

	/**
		Compares 2 strings
		@return number of different characters (including difference in lengths)
	*/
	static inline uint32_t difference(const std::string& str, const std::string& source) {
		size_t slength;
		uint32_t diff;
		if (str.length() > source.length()) {
			slength = source.length();
			diff = static_cast<uint32_t>(str.length() - source.length());
		}
		else {
			slength = str.length();
			diff = static_cast<uint32_t>(source.length() - str.length());
		}

		for (size_t i = 0; i < slength; i++) {
			if (str[i] != source[i])
				diff++;
		}

		return diff;
	}

	/**
		Structure for returning success values with messages
		Automatically deletes message, because it assumes the message is not reused and there is no point copying it
	*/
	struct Success {
		/**
		@param success default true
		@param message no need for message when passed (default "")
		*/
		Success(const bool& success = true, const std::string& message = "") {
			this->message = message;
			this->success = success;
		}

		operator bool() {
			return success;
		}

		bool success;
		std::string message;
	};
}

