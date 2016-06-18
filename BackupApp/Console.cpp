#include "stdafx.h"
#include "Console.h"

using std::string;

Console::Console(const uint16_t& length, const uint16_t& spaces) {
	if (length == 0)
		throw std::runtime_error("Length cannot be 0");
	this->columnsCount = length;
	this->columns = new uint16_t[length];
	for (size_t i = 0; i < length; i++)
		this->columns[i] = 0;
	this->spaces = spaces;
}


Console::~Console() {
	delete[] columns;
	for (size_t i = 0; i < content.size(); i++)
		delete[] content[i];
}

Console& Console::Add(const std::string &text) {
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
	return *this;
}

Console& Console::Add(const std::vector<std::string>& vec) {
	for (auto line : vec)
		Add(line);
	return *this;
}

Console& Console::Add(const std::vector<std::string*>& vec) {
	for (auto line : vec)
		Add(*line);
	return *this;
}

void Console::Print(const bool showIndexes)const {
	for (size_t i = 0; i < content.size(); i++) {
		auto row = content[i];
		if (showIndexes) {
			int size = ext::numDigits(content.size());
			auto index = std::to_string(i);
			std::cout << index << std::string(size - index.length() + spaces, ' ');
		}
		for (size_t ii = 0; ii < columnsCount; ii++)
			std::cout << row[ii] << std::string(columns[ii] - row[ii].size() + spaces, ' ');
		std::cout << std::endl;
	}
}

void Console::PrintError(const std::string & message) {
	std::cout << std::endl << "#### Error ####" << std::endl << message << std::endl << std::endl;
}

void Console::PrintWarning(const std::string & message) {
	std::cout << std::endl << ":::: Warning ::::" << std::endl << message << std::endl << std::endl;
}
