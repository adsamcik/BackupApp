#include "stdafx.h"
#include "Console.h"


Console::Console(const uint16_t& length, const uint16_t& spaces) {
	if (length == 0)
		throw std::exception("Length cannot be 0");
	this->columnsLength = length;
	this->columns = new uint16_t[length];
	for (size_t i = 0; i < length; i++) {
		this->columns[i] = 0;
	}
	this->spaces = spaces;
}


Console::~Console() {
	delete[] columns;
}

void Console::AddLine(const std::string & text) {
	size_t prev = 0, last = 0;
	for (size_t i = 0; i < columnsLength; i++) {
		if (i + 1 == columnsLength)
			last = text.length();
		else
			last = text.find('\t', last);
		if (last == std::string::npos)
			throw std::invalid_argument("text has less rows than required");
		if (columns[i] < last - prev - 1)
			columns[i] = static_cast<short>(last - prev - 1);
		prev = last++;
	}
	content.push_back(text);
}

void Console::Print() {
	for (size_t i = 0; i < content.size(); i++) {
		size_t tabIndex = 0;
		size_t nextTab = 0;
		auto c = content[i];
		for (size_t ii = 0; ii < columnsLength; ii++) {
			if (ii + 1 == columnsLength)
				nextTab = c.length();
			else
				nextTab = c.find('\t', tabIndex);

			auto substr = c.substr(tabIndex, nextTab);
			std::cout << substr << std::string(columns[ii] - substr.size() + spaces, ' ');
			tabIndex = nextTab;
		}
		std::cout << std::endl;
	}
}

void Console::PrintError(const std::string & message) {
	std::cout << std::endl << "#### Error ####" << std::endl << message << std::endl << std::endl;
}
