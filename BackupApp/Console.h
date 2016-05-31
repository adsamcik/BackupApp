#include "stdafx.h"
#include <vector>
#pragma once

///Used to print columns to output
class Console {
public:
	/** Constructor
	* @param columnCount number of columns
	* @param spaces number of spaces between columns
	*/
	Console(const uint16_t& columnCount, const uint16_t& spaces = 4);
	~Console();
	/** Add new line to console
	* @param text Line to be displayed, columns seperated with tabs (\t)
	*/
	void AddLine(const std::string& text);

	/** Prints all added lines to cout
	*/
	void Print();

	static void PrintError(const std::string& message);

#ifdef _WIN32
	static inline void Clear() { std::system("cls"); }
#elif __linux__
	static inline void Clear() { std::system("clear"); }
#endif
private:
	///Number of columns
	uint16_t columnsLength;
	///Array of max number of chars in column
	uint16_t* columns;
	///Number of seperation spaces
	uint16_t spaces;
	///Actual strings
	std::vector<std::string> content;
};

