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

	static inline std::string& tolower(std::string& s) {
		for (size_t i = 0; i < s.length(); i++)
			s[i] = std::tolower(s[i]);
		return s;
	}

	//trim right
	static inline std::string& rtrim(std::string& s, const char* t = ws) {
		return s.erase(s.find_last_not_of(t) + 1);;
	}

	// trim left
	static inline std::string& ltrim(std::string& s, const char* t = ws) {
		return s.erase(0, s.find_first_not_of(t));
	}

	// trim both
	static inline std::string& trim(std::string& s, const char* t = ws) {
		return ltrim(rtrim(s, t), t);
	}

	/// Checks if string starts with set characters 
	static bool startsWith(const std::string& string, const std::string& what) {
		if (string.length() < what.length())
			return false;
		for (size_t i = 0; i < what.length(); i++) {
			if (string[i] != what[i])
				return false;
		}
		return true;
	}

	static inline bool isDigit(const std::string &str) {
		return std::all_of(str.begin(), str.end(), ::isdigit);
	}

	static inline bool isDir(const char* path) {
		struct stat path_stat;
		stat(path, &path_stat);
		return (path_stat.st_mode & S_IFDIR) == S_IFDIR;
	}

	static inline bool isValidPath(const char* path) {
		struct stat info;
		return stat(path, &info) == 0;
	}

	static inline bool isValidPath(const std::string& path) {
		return isValidPath(path.c_str());
	}

	/**
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
		///Default state is true without message, because the name of this structure is Success
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

