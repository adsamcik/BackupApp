#include "stdafx.h"
#include <vector>
#pragma once

///Used to print columns to output
class Console {
public:
	/** Constructor
		@param columnCount number of columns
		@param spaces number of spaces between columns
	*/
	Console(const uint16_t& columnCount, const uint16_t& spaces = 4);
	~Console();

	/** 
		Add new line to console
		@param text Line to be displayed, columns seperated with tabs (\\t)
	*/
	Console& Add(const std::string& text);

	/**
	Add multiple new line to console
	@param text Line to be displayed, columns seperated with tabs (\\t)
	*/
	Console& Add(const std::vector<std::string>& vec);

	/**
	Add multiple new line to console
	@param text Line to be displayed, columns seperated with tabs (\\t)
	*/
	Console& Add(const std::vector<std::string*>& vec);

	/** 
		Prints all added lines to cout
		@param showIndexes show index values in the first column
	*/
	void Print(const bool showIndexes) const;

	static void PrintError(const std::string& message);
	static void PrintWarning(const std::string& message);

	static inline void Clear() {
#ifdef _WIN32 
		std::system("cls");
#elif __linux__
		std::system("clear");
#endif
	}

private:
	///Number of columns
	uint16_t columnsCount;
	///Array of max number of chars in column
	uint16_t* columns;
	///Number of seperation spaces
	uint16_t spaces;
	///Actual strings
	std::vector<std::string*> content;
};

