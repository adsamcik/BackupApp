#pragma once
#include "stdafx.h"
#include <vector>

///Used to print columns to output
class Console {
public:
	/** Constructor 
	* @param columnCount number of columns
	* @param spaces number of spaces between columns
	*/
	Console(const uint16_t& columnCount, const uint16_t& spaces);
	~Console();
	void AddLine(const std::string& text);
	void Print();
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

