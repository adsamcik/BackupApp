#include "stdafx.h"
#include <cctype>
#include <algorithm>
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#endif
#pragma once

#define ws " \t\n\r\f\v"

using std::string;

/**
	Namespace ext is generally list of short functions that are used all around the app
	Functions shorter or equal to 3 lines are inline
	because it's still easy to read and it's quite short anyway
*/
namespace ext {
	/**
		DayOfWeek starting with Monday
		Keeps actual day of the week values (1-7)
		Value 0 (undefined) is used as null
	*/
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
		@return lower case source string
	*/
	static inline string& tolower_r(string& s) {
		for (size_t i = 0; i < s.length(); i++)
			s[i] = std::tolower(s[i]);
		return s;
	}

	/**
		Creates new lowercase string from source string
		@param s source string
		@return lowercase copy of source string
	*/
	static inline string tolower(const string& s) {
		string ns = s;
		for (size_t i = 0; i < ns.length(); i++)
			ns[i] = std::tolower(ns[i]);
		return ns;
	}

	/**
		Trims right side of string from white characters by default
		@param s string to trim from the right
		@param t white characters which should be trimmed (default " \t\n\r\f\v")
	*/
	static inline string& rtrim(string& s, const char* t = ws) {
		return s.erase(s.find_last_not_of(t) + 1);;
	}

	/**
	Trims left side of string from white characters by default
	@param s string to trim from the left
	@param t white characters which should be trimmed (default " \t\n\r\f\v")
	*/
	static inline string& ltrim(string& s, const char* t = ws) {
		return s.erase(0, s.find_first_not_of(t));
	}

	/**
	Trims white characters on both sides of the string
	@param s string to trim
	@param t white characters which should be trimmed (default " \t\n\r\f\v")
	*/
	static inline string& trim(string& s, const char* t = ws) {
		return ltrim(rtrim(s, t), t);
	}

	/**
	Checks if string starts with another string
	@param string string to check
	@param what what should the string start with
	*/
	static bool startsWith(const string& str, const string& what) {
		if (str.length() < what.length())
			return false;
		for (size_t i = 0; i < what.length(); i++) {
			if (str[i] != what[i])
				return false;
		}
		return true;
	}

	/**
		Number of digits in number
	*/
	template <class T>
	int numDigits(T number) {
		int digits = 0;
		if (number < 0) digits = 1; // remove this line if '-' counts as a digit
		while (number) {
			number /= 10;
			digits++;
		}
		return digits;
	}

	/**
	Checks if all characters in string are digits
	@param str string to check
	*/
	static inline bool isDigit(const string &str) {
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
	static inline bool isValidPath(const string& path) {
		return isValidPath(path.c_str());
	}

	static inline string parent(const string& path) {
		auto index = path.find_last_of("/\\");
		if (index == string::npos)
			return "\\";
		return path.substr(0, index);
	}

	static inline string fullPath(const string relPath) {
		char* p;
#ifdef _WIN32 
		p = new char[4096];
		_fullpath(p, relPath.c_str(), 4096);
#elif linux
		realpath(s1.c_str(), p);
#endif
		string ret = string(p);
		delete[] p;
		return ret;
	}

	/**
	Compares 2 paths
	Accurate check should be ran on linux
	Contains only simple check on Windows due to requirement of rather bigger lib to ensure crossplatform compatibility
	*/
	static inline bool comparePaths(const string &s1, const string &s2) {
		char *r1, *r2;
#ifdef _WIN32 
		r1 = new char[4096];
		r2 = new char[4096];
		_fullpath(r1, s1.c_str(), 4096);
		_fullpath(r2, s2.c_str(), 4096);
#elif linux
		realpath(s1.c_str(), r1);
		realpath(s2.c_str(), r2);
#endif
		bool areTheSame = strcmp(r1, r2) == 0;
		delete[] r1;
		delete[] r2;
		return areTheSame;
	}

	/**
		Compares 2 strings
		@return number of different characters (including difference in lengths)
	*/
	static inline uint32_t difference(const string& str, const string& source) {
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
		message is not a pointer to reduce copying when it's not null
	*/
	struct Success {
		Success(const Success& val) {
			this->message = new string(*val.message);
			this->success = val.success;
		}

		Success() {
			this->success = true;
			this->message = nullptr;
		}

		Success(const bool& success, const string& message) {
			this->message = new string(message);
			this->success = success;
		}

		~Success() {
			if (message != nullptr)
				delete message;
		}

		bool operator!() {
			return success;
		}

		bool success;
		string *message;
	};
}

