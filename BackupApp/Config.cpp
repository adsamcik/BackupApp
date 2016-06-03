#include "stdafx.h"
#include "Config.h"
#include "Console.h"


Config::Config() {
	std::ifstream stream(CONFIG);
	std::string line;

	std::getline(stream, line);
	if (line.empty())
		return;
	try {
		this->day = static_cast<ext::DayOfWeek>(std::stoi(line));
	}
	catch(const std::exception e) {
		Console::PrintError(e.what());
		return;
	}

	while (std::getline(stream, line)) {
		std::istringstream iss(line);
		bool isDir;
		std::string path;
		if (!(iss >> isDir >> path)) { break; }
		AddPath(path);
	}
}

Config::~Config() {}

ext::Success Config::AddPath(const std::string & path) {
	return ext::Success();
}

ext::Success Config::RemovePath(const std::string & path) {
	return ext::Success();
}

ext::Success Config::Save() {
	std::ofstream stream(CONFIG);
	if (!stream)
		return ext::Success(false, "Unable to write to config file");

	stream << day << std::endl;
	for (size_t i = 0; i < paths.size(); i++)
		stream << paths[i] << std::endl;
	stream.close();
	return ext::Success();
}

void Config::Edit() {
	Console::Clear();
	std::cout << "OPTIONS" << std::endl;
	PrintOptions();
	while (true) {
		std::string s;
		std::string sLower;
		std::cin >> s;
		Console::Clear();
		s = ext::trim(s);
		ext::tolower(sLower = s);

		if (ext::startsWith(sLower, "day=")) {
			try {
				int val = std::stoi(sLower.substr(4));
				std::cout << "Variable day updated" << std::endl;
			}
			catch(std::exception e) {
				std::cout << "Variable day needs a number" << std::endl;
			}
		}
	}
}

void Config::PrintOptions() {
	Console c(2);
	c.AddLine("day=<0-7>\tsets day of auto-backup. 0 disables autobackup, 1 is Monday");
	c.AddLine("help\tto print this help again");
	c.Print();
}
