#include "stdafx.h"
#include "Config.h"
#include "Console.h"
#include <sys/stat.h>
#include <sys/types.h>

using std::string;

void Config::Initialize() {
	std::ifstream stream(CONFIG_FILE);
	string line;

	getline(stream, line);
	if (line.empty())
		return;
	try {
		auto frst = line.find_first_of('=');
		if (frst == string::npos)
			throw std::exception("Day is incorrectly saved");
		day = static_cast<ext::DayOfWeek>(std::stoi(line.substr(frst + 1)));
	}
	catch (const std::exception e) {
		Console::PrintError(e.what());
		return;
	}

	while (getline(stream, line)) {
		if (ext::startsWith(line, "#"))
			continue;
		if (!ext::isValidPath(line))
			Console::PrintError(line + " is invalid path");
		else
			AddPath(line);
	}
}

const ext::Success Config::AddPath(const string & path) {
	auto fullPath = ext::fullPath(path);
	for (size_t i = 0; i < paths.size(); i++) {
		if (ext::comparePaths(fullPath, paths[i]))
			return ext::Success(false, "Path is already backed up");
	}
	paths.push_back(fullPath);
	return ext::Success();
}

const ext::Success Config::RemovePath(const string & path) {
	return ext::Success();
}

const ext::Success Config::RemovePath(const size_t & index) {
	if (index < paths.size()) {
		paths.erase(paths.begin() + index);
		Save();
		return ext::Success();
	}
	return ext::Success(false, "Invalid index");
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
		std::cout << "::CONFIG::";
		string s;
		string in;
		std::vector<string> input;
		getline(std::cin, in);
		//Console::Clear();
		std::istringstream tss(in);
		while (tss >> s)
			input.push_back(s);

		if (input.size() == 0) {
			Console::PrintError("No command detected");
			continue;
		}

		string cmd = ext::tolower(input[0]);

		if (ext::startsWith(cmd, "day")) {
			if (input.size() != 2)
				Console::PrintError("Invalid number of arguments. Required 1 number.");
			else
				USetDay(input[1]);
		}
		else if (cmd == "list") {
			UList();
		}
		else if (ext::startsWith(cmd, "add")) {
			if (input.size() < 2)
				Console::PrintError("You did not enter path");
			else
				UAdd(in.substr(4));
		}
		else if (ext::startsWith(cmd, "remove")) {
			if (input.size() < 3)
				Console::PrintError("Too few arguments. See help.");
			else if (input[1] == "-p") {
				string spath;
				auto index = in.find_first_of("-p");
				spath = ext::ltrim(in.substr(index + 2));
				URemove(spath);
			}
			else if (input[1] == "-i") {
				if (input.size() > 3)
					Console::PrintError("Too many arguments");
				else if (!ext::isDigit(input[3]))
					Console::PrintError("Argument must be a number");
				else {
					int val = atoi(input[3].c_str());
					if (val >= paths.size() || val < 0)
						Console::PrintError("Index must be between 0 and " + std::to_string(paths.size()));
					else
						URemove(val);
				}
			}
			else {
				Console::PrintError("After remove you must choose whether you use path (-p) or index (-i). See help.");
			}
		}
		else if (ext::startsWith(cmd, "return"))
			break;
		else if (ext::startsWith(cmd, "exit"))
			exit(0);
		else
			std::cout << "Unknown command" << std::endl;
	}
}

void Config::USetDay(const string & params) {
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
			throw std::exception(save.message->c_str());
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
		std::cout << e.what() << std::endl;
	}
}

void Config::UList() {
	if (paths.size() == 0)
		std::cout << "Nothing is being backed up." << std::endl;
	else {
		Console c(2);
		for (size_t i = 0; i < paths.size(); i++)
			c.AddLine(std::to_string(i) + "\t" + paths[i]);
		c.Print();
	}
}

void Config::UAdd(const string & path) {
	if (!ext::isValidPath(path)) {
		Console::PrintError("Invalid path");
		return;
	}
	char last = path[path.length() - 1];
	auto s = AddPath(last == '/' || last == '\\' ? path.substr(0, path.length() - 1) : path);
	if (!s.success)
		Console::PrintError(*s.message);
	else
		Save();
}

void Config::URemove(const string & line) {
	std::vector<string*> closeMatches;
	for (size_t i = 0; i < paths.size(); i++) {
		auto lower = ext::tolower(paths[i]);
		auto diff = lower.find(line);
		if (diff != string::npos) {
			if (paths[i] == line) {
				paths.erase(paths.begin() + i);
				std::cout << "Removed successfully" << std::endl;
				return;
			}
			else
				closeMatches.push_back(&paths[i]);
		}

	}

	if (closeMatches.size() == 0)
		std::cout << "Did not find any matches" << std::endl;
	else if (closeMatches.size() == 1) {
		std::cout << "Did you mean " << *closeMatches[0] << " yes/NO" << std::endl;
		string response;
		getline(std::cin, response);
		ext::tolower_r(response);
		if (response == "yes")
			URemove(*closeMatches[0]);
		else
			return;
	}
	else {
		std::cout << std::endl << "Found these close matches. Use index to pick which one you want to remove anything else to skip." << std::endl;
		Console c(2);
		for (size_t i = 0; i < closeMatches.size(); i++)
			c.AddLine(std::to_string(i) + '\t' + *closeMatches[i]);
		c.Print();
		string response;
		getline(std::cin, response);
		if (ext::isDigit(response)) {
			auto val = atoi(response.c_str());
			if (val > 0 && val < closeMatches.size())
				URemove(*closeMatches[val]);
			else
				std::cout << "Invalid index. Ending." << std::endl;
		}
		else
			std::cout << "No index detected. Ending." << std::endl;

	}

}

void Config::URemove(const size_t & index) {
	paths.erase(paths.begin() + index);
}

void Config::PrintOptions() {
	Console c(2);
	c.AddLine("day <0-7>\tsets day of auto-backup. 0 disables autobackup, 1 is Monday");
	c.AddLine("list\treturns list of backed up folders and files with their indexes for easier removal");
	c.AddLine("add <path>\tadds path to backup");
	c.AddLine("remove <-p/-i> <path/index>\tremoves path. Use -p for path or -i for index");
	c.AddLine("help\tto print this help again");
	c.Print();
}

std::vector<string> Config::paths;
ext::DayOfWeek Config::day;