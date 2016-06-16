#include "stdafx.h"
#include "Console.h"

using std::string;

Console::Console(const uint16_t& length, const uint16_t& spaces) {
	if (length == 0)
		throw std::exception("Length cannot be 0");
	this->columnsCount = length;
	this->columns = new uint16_t[length];
	for (size_t i = 0; i < length; i++)
		this->columns[i] = 0;
	this->spaces = spaces;
}


Console::~Console() {
	delete[] columns;
}

void Console::AddLine(const std::string & text) {
	size_t prev = 0, last = 0;
	string* arr = new string[columnsCount];
	for (size_t i = 0; i < columnsCount; i++) {
		if (i + 1 == columnsCount)
			last = text.length();
		else
			last = text.find('\t', last);
		if (last == string::npos)
			throw std::invalid_argument("text has less rows than required");

		arr[i] = text.substr(prev, last - prev);

		size_t lastprev = last - prev;
		if (columns[i] < lastprev)
			columns[i] = static_cast<short>(lastprev);
		prev = last++;
	}
	content.push_back(arr);
}

void Console::Print() {
	for (auto r : content) {
		size_t tabIndex = 0;
		size_t nextTab = 0;
		for (size_t i = 0; i < columnsCount; i++) {
			std::cout << r[i] << std::string(columns[i] - r[i].size() + spaces, ' ');
			tabIndex = nextTab;
		}
		std::cout << std::endl;
	}
}

void Console::PrintError(const std::string & message) {
	std::cout << std::endl << "#### Error ####" << std::endl << message << std::endl << std::endl;
}
