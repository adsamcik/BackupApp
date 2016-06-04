#include "stdafx.h"
#include "Config.h"
#include "Console.h"
#include <sys/stat.h>
#include <sys/types.h>


Config::Config() {
	std::ifstream stream(CONFIG);
	std::string line;

	std::getline(stream, line);
	if (line.empty())
		return;
	try {
		auto frst = line.find_first_of('=');
		if (frst == std::string::npos)
			throw std::exception("Day is incorrectly saved");
		this->day = static_cast<ext::DayOfWeek>(std::stoi(line.substr(frst + 1)));
	}
	catch (const std::exception e) {
		Console::PrintError(e.what());
		return;
	}

	while (std::getline(stream, line)) {
		if (ext::startsWith(line, "#"))
			continue;
		std::ifstream test(line);
		if (!test)
			Console::PrintError(line + " is invalid path");
		else {
			struct stat path_stat;
			stat(line.c_str(), &path_stat);
				AddPath(line, S_I);
		}
	}
}

Config::~Config() {}

const ext::Success& Config::AddPath(const std::string & path) {
	auto s = ext::Success();
	return s;
}

const ext::Success& Config::RemovePath(const std::string & path) {
	auto s = ext::Success();
	return s;
}

const ext::Success& Config::Save() {
	std::ofstream stream(CONFIG);
	std::cout << CONFIG;
	if (!stream)
		return ext::Success(false, "Unable to write to config file");

	stream << "day=" << day << std::endl;
	stream << "#PATHS TO BACKUP" << std::endl;
	for (size_t i = 0; i < paths.size(); i++)
		stream << paths[i] << std::endl;
	stream.close();
	auto s = ext::Success();
	return s;
}

void Config::Edit() {
	Console::Clear();
	std::cout << "OPTIONS" << std::endl;
	PrintOptions();
	while (true) {
		std::string s;
		std::string sLower;
		std::cin >> s;
		//Console::Clear();
		s = ext::trim(s);
		ext::tolower(sLower = s);

		if (ext::startsWith(sLower, "day")) {
			if (sLower.length() > 3) {
				if (sLower.length() == 4)
					std::cout << "Number cannot be empty" << std::endl;
				else if (sLower[3] != '=')
					std::cout << "Unknown command" << std::endl;
				else {
					auto tmp = day;
					try {
						auto substr = sLower.substr(4);
						if (!ext::is_digits(substr))
							throw std::invalid_argument("");
						auto d = std::stoi(substr);
						if (d < 0 || d > 7)
							throw std::out_of_range("Number is out of range");
						day = static_cast<ext::DayOfWeek>(d);
						if (!Save().success)
							throw std::exception();
						std::cout << "Day updated" << std::endl;
					}
					catch (std::invalid_argument e) {
						std::cout << "Day needs a number" << std::endl;
					}
					catch (std::out_of_range e) {
						day = tmp;
						std::cout << "Number must be in range 0-7" << std::endl;
					}
					catch (std::exception e) {
						day = tmp;
						std::cout << "Variable day needs a number" << std::endl;
					}
				}
			}
		}
		else if (ext::startsWith(sLower, "exit") || ext::startsWith(sLower, "return"))
			break;
		else {
			std::cout << "Unknown command" << std::endl;
		}
	}
}

void Config::SetDay(const int day) {
	if (day > 0 && day < 8)
		this->day = static_cast<ext::DayOfWeek>(day);
	else
		this->day = ext::Undefined;
}

void Config::PrintOptions() {
	Console c(2);
	c.AddLine("day=<0-7>\tsets day of auto-backup. 0 disables autobackup, 1 is Monday");
	c.AddLine("help\tto print this help again");
	c.Print();
}
