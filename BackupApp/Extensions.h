#include "stdafx.h"
#include <cctype>
#pragma once

#define ws " \t\n\r\f\v"
namespace ext {
	enum DayOfWeek {
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
		s.erase(s.find_last_not_of(t) + 1);
		return s;
	}

	// trim left
	static inline std::string& ltrim(std::string& s, const char* t = ws) {
		s.erase(0, s.find_first_not_of(t));
		return s;
	}

	// trim both
	static inline std::string& trim(std::string& s, const char* t = ws) {
		return ltrim(rtrim(s, t), t);
	}

	static inline bool startsWith(const std::string& string, const std::string& what) {
		if (string.length() < what.length())
			return false;
		for (size_t i = 0; i < what.length(); i++) {
			if (string[i] != what[i])
				return false;
		}
		return true;
	}

	/**
		Structure for returning success values with messages
		Automatically deletes message, because it assumes the message is not reused and there is no point copying it
	*/
	struct Success {
		///Default state is true without message, because the name of this structure is Success
		Success(const bool& success = true, char* message = "") {
			this->message = message;
			this->success = success;
		}

		~Success() {
			delete[] message;
		}

		operator bool() {
			return success;
		}

		bool success;
		char* message;
	};
}

