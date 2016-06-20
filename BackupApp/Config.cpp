#include "stdafx.h"
#include "Config.h"
#include "Console.h"
#include <sys/stat.h>
#include <sys/types.h>

using std::string;

void Config::Initialize() {
	if(!ext::isValidPath(ext::fullPath(CONFIG_FILE))) {
		std::cout << "Config file does not exists. Creating." << std::endl;
		std::ofstream ostream(CONFIG_FILE);
		if (!ostream.is_open()) {
			std::cout << "Failed to create config file. Aborting." << std::endl;
			return;
		}
		ostream.close();
	}
	std::ifstream stream(CONFIG_FILE);
	if (!stream.is_open()) {
		Console::PrintError("Failed to open config file");
		return;
	}

	string line;
	getline(stream, line);
	if (line.empty())
		return;
	try {
		if (ext::trim(line).substr(0, 4) != "day=" || line.length() < 5)
			throw std::runtime_error("Day is incorrectly saved. Config was not loaded. If you update config with new path or day it might override the current config.");
		auto ret = USetDay(line.substr(4));
		if(!ret)
			throw std::runtime_error(ret.message->c_str());		
	}
	catch (const std::exception e) {
		Console::PrintError(e.what());
		return;
	}

	paths.clear();
	while (getline(stream, line)) {
		if (ext::startsWith(line, "#"))
			continue;
#ifdef _WIN32
		ext::trim(line);
#endif
		UAdd(line);
	}
}

const ext::Success Config::AddPath(const string & path) {
	auto fullPath = ext::fullPath(path);
	for (size_t i = 0; i < paths.size(); i++) {
		if (ext::comparePaths(fullPath, paths[i]))
			return ext::Success(false, "Path is already backed up");
	}
	paths.push_back(fullPath);
	std::sort(paths.begin(), paths.end());
	return ext::Success();
}

const ext::Success Config::RemovePath(const std::string & path) {
	for (size_t i = 0; i < paths.size(); i++) {
		if (paths[i] == path) {
			RemovePath(i);
			return ext::Success();
		}
	}
	return ext::Success(false, "path not found");
}

const ext::Success Config::RemovePath(const size_t & index) {
	if (index < paths.size()) {
		paths.erase(paths.begin() + index);
		Save();
		return ext::Success();
	}
	return ext::Success(false, "Invalid index");
}

ext::DayOfWeek Config::GetDay() {
	return day;
}

const ext::Success Config::Save() {
	std::ofstream stream(CONFIG_FILE);
	if (!stream)
		return ext::Success(false, "Unable to write to config file");

	stream << "day=" << day << std::endl;
	stream << "#PATHS TO BACKUP" << std::endl;
	for (size_t i = 0; i < paths.size(); i++)
		stream << paths[i] << std::endl;
	stream.close();
	return ext::Success();
}

void Config::Edit(FileManager &fm) {
	Console::Clear();
	std::cout << "OPTIONS" << std::endl;
	PrintOptions();
	while (true) {
		std::cout << std::endl << "::CONFIG:: ";
		string s;
		string in;
		std::vector<string> input;
		getline(std::cin, in);
		std::istringstream tss(in);
		while (tss >> s)
			input.push_back(s);

		if (input.size() == 0) {
			Console::PrintError("No command detected");
			continue;
		}

		string cmd = ext::tolower(input[0]);

		if (cmd == "day") {
			if (input.size() == 1) {
				if (day == ext::DayOfWeek::Undefined)
					std::cout << "You have not set backup day yet." << std::endl;
				else {
					std::cout << "Your files will be backed up every " << ext::dayOfWeekToString(day) << ". If you don't forget to launch this app." << std::endl;
				}
			}
			else if (input.size() > 2)
				Console::PrintError("Invalid number of arguments. Required 1 number.");
			else {
				auto ret = USetDay(input[1]);
				if (!!ret)
					std::cout << "Day updated" << std::endl;
			}
		}
		else if (cmd == "list") {
			UList();
		}
		else if (cmd == "add") {
			if (input.size() < 2)
				Console::PrintError("You did not enter path");
			else
				UAdd(in.substr(4));
		}
		else if (cmd == "remove") {
			if (input.size() < 3)
				Console::PrintError("Too few arguments. See help.");
			else if (input[1] == "-p") {
				string spath;
				auto index = in.find_first_of("-p");
				auto substr = in.substr(index + 2);
				spath = ext::trim(substr);
				URemove(fm, spath);
			}
			else if (input[1] == "-i") {
				if (input.size() > 3)
					Console::PrintError("Too many arguments");
				else if (!ext::isDigit(input[2]))
					Console::PrintError("Argument must be a number");
				else {
					int val = std::stoi(input[2].c_str());
					if (val >= static_cast<int>(paths.size()) || val < 0)
						Console::PrintError("Index must be between 0 and " + std::to_string(paths.size() == 0 ? 0 : paths.size() - 1));
					else
						URemove(fm, val);
				}
			}
			else {
				Console::PrintError("After remove you must choose whether you use path (-p) or index (-i). See help.");
			}
		}
		else if (cmd == "refresh")
			Initialize();
		else if (cmd == "help")
			PrintOptions();
		else if (cmd == "return")
			break;
		else if (cmd == "exit")
			exit(0);
		else
			std::cout << "Unknown command" << std::endl;
	}
}

ext::Success Config::USetDay(const string & params) {
	auto tmp = day;
	try {
		if (!ext::isDigit(params))
			throw std::invalid_argument("");
		auto d = std::stoi(params);
		if (d < 0 || d > 7)
			throw std::out_of_range("Number is out of range");
		day = static_cast<ext::DayOfWeek>(d);
		auto save = Save();
		if (!save.success)
			throw std::runtime_error(save.message->c_str());
	}
	catch (std::invalid_argument e) {
		std::cout << "Day needs a number. 1 is Monday, 7 is Sunday, 0 is no day." << std::endl;
		return ext::Success(false, "invalid argument");
	}
	catch (std::out_of_range e) {
		day = tmp;
		std::cout << "Day number must be in range 0-7" << std::endl;
		return ext::Success(false, "out of range");
	}
	catch (std::exception e) {
		day = tmp;
		std::cout << e.what() << std::endl;
		return ext::Success(false, e.what());
	}
	return ext::Success();
}

void Config::UList() {
	if (paths.size() == 0)
		std::cout << "Nothing is being backed up." << std::endl;
	else {
		Console c(1);
		c.Add(paths).Print(true);
	}
}

void Config::UAdd(const string & path) {
	string temp = path;
	if (!ext::isValidPath(path)) {
		Console::PrintError("Invalid path");
		return;
	}
#ifdef __linux
	else if (ext::trim(temp) == "/") {
		Console::PrintError("Sorry, you can't backup root of the file system.");
		return;
	}
#endif

	char last = path[path.length() - 1];
	auto s = AddPath(last == SEPARATOR ? path.substr(0, path.length() - 1) : path);
	if (!s.success)
		Console::PrintError(*s.message);
	else
		Save();
}

void Config::URemove(FileManager &fm, const string &line) {
	std::vector<string*> closeMatches;
	ext::findCloseMatch(paths, line, closeMatches, false);

	if (closeMatches.size() == 0)
		std::cout << "Did not find any matches" << std::endl;
	else if (closeMatches.size() == 1) {
		std::cout << "Did you mean " << *closeMatches[0] << " yes/NO" << std::endl;
		string response;
		getline(std::cin, response);
		ext::tolower_r(response);
		if (response == "yes") {
			string path = *closeMatches[0];
			RemovePath(path);
			fm.Remove(path);
		}
		else
			return;
	}
	else {
		std::cout << std::endl << "Write index of path you want to remove. Invalid index will abort removal." << std::endl;
		Console c(1);
		c.Add(closeMatches).Print(true);
		string response;
		getline(std::cin, response);
		if (ext::isDigit(response)) {
			int val = -1;
			try {
				val = std::stoi(response.c_str());
			}
			catch (std::out_of_range e) {
				Console::PrintError("Index is out of range! This can't be right.");
			}
			if (val >= 0 && val < static_cast<int>(closeMatches.size())) {
				string path = *closeMatches[val];
				RemovePath(path);
				fm.Remove(path);
			}
			else
				std::cout << "Invalid index. Ending." << std::endl;
		}
		else
			std::cout << "No index detected. Ending." << std::endl;

	}

}

void Config::URemove(FileManager &fm, const size_t & index) {
	fm.Remove(paths[index]);
	RemovePath(index);
}

void Config::PrintOptions() {
	Console c(2);
	c.Add("day <0-7>\tsets day of auto-backup. 0 disables autobackup, 1 is Monday. Returns current settings without argument.")
		.Add("list\treturns list of backed up folders and files with their indexes for easier removal")
		.Add("add <path>\tadds path to backup")
		.Add("remove <-p/-i> <path/index>\tremoves path. Use -p for path or -i for index")
		.Add("refresh\trefreshes config settings from the file")
		.Add("help\tto print this help again")
		.Add("return\tto go back from the configurations")
		.Add("exit\tto exit the app")
		.Print(false);
}

std::vector<string> Config::paths;
ext::DayOfWeek Config::day;