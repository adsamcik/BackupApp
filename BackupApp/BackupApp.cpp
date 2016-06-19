#include "stdafx.h"
#include "Console.h"
#include "Config.h"
#include "FileManager.h"
#include <ctime>

using std::string;

FileManager fm;

/// <summary>Handles command recognition from user input
/// </summary>
bool ResolveInput() {
	std::cout << ":: ";

	std::vector<string> input;
	string s;
	string in;
	std::getline(std::cin, in);
	std::istringstream tss(in);
	while (tss >> s)
		input.push_back(s);

	if (input.size() == 0) {
		Console::PrintError("No command detected");
		return true;
	}

	string cmd = ext::tolower(input[0]);

	if (cmd == "backup") {
		if (input.size() > 1 && input[1] == "-force")
			std::cout << "force" << std::endl;
		fm.BackupAll();
	}
	else if (cmd == "restore") {
		if (input.size() >= 3) {
			string substr = ext::trim(in).substr(11);
			if (input[1] == "-d")
				fm.Restore(substr, true);
			else if (input[1] == "-f")
				fm.Restore(substr, false);
			else
				Console::PrintError("Missing operator -f or -d");
		}
		else
			Console::PrintError("Invalid number of arguments");
	}
	else if (cmd == "clear")
		fm.Clear();
	else if (cmd == "exit")
		return false;
	else if (cmd == "config") {
		Config::Edit(fm);
	}
	else if (cmd == "help") {
		Console cns(2, 4);
		cns.Add("backup\tbacks up all paths")
			.Add("\t-force forces full backup")
			.Add("restore <-f/-d> <path>\tsupports relative and absolute paths (-f file, -d directory)")
			.Add("clear\tclears all backups")
			.Add("config\tto access configurations")
			.Add("exit\tto close the app")
			.Print(false);
	}
	else if (cmd == "test")
		fm.PrintContent(20);
	else 
		std::cout << cmd << " is not a command. Type 'help' for list of commands" << std::endl << std::endl;

	std::cout << std::endl;
	return true;
}

int main() {
	Console::Clear();
	Config::Initialize();

	auto d = Config::GetDay();
	time_t timev;
	time(&timev);
	tm* timeday = new tm();
#ifdef _WIN32
	localtime_s(timeday, &timev);
#else
	localtime_r(&timev, timeday);
#endif
	ext::DayOfWeek day = timeday->tm_wday == 0 ? ext::DayOfWeek::Sunday : static_cast<ext::DayOfWeek>(timeday->tm_wday);
	if (d == day)
		fm.BackupAll();
	delete timeday;

	while (ResolveInput()) {}
	return 0;
}