#include "stdafx.h"
#include "Console.h"
#include "Config.h"
#include "FileManager.h"

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

	if (ext::startsWith(cmd, "backup")) {
		if (input.size() > 1 && input[1] == "-force")
			std::cout << "force" << std::endl;
		fm.BackupAll();
	}
	else if (ext::startsWith(cmd, "restore")) {
		if (input.size() >= 3) {
			string substr = ext::trim(in).substr(11);
			if (input[1] == "-d")
				fm.Restore(ext::fullPath(substr), true);
			else if (input[1] == "-f")
				fm.Restore(substr, false);
			else
				Console::PrintError("Missing operator -f or -d");
		}
		else
			Console::PrintError("Invalid number of arguments");
	}
	else if (ext::startsWith(cmd, "clear"))
		fm.Clear();
	else if (ext::startsWith(cmd, "exit")) {
		if (cmd != "exit") {
			std::cout << "Did you really mean to exit? (yes/NO)" << std::endl;
			while (true) {
				std::getline(std::cin, s);
				ext::tolower_r(s);
				if (ext::startsWith(s, "yes"))
					return false;
				else if (ext::startsWith(s, "no")) {
					std::cout << std::endl;
					return true;
				}

				std::cout << s << " is not a valid answer to yes or no question." << std::endl;
			}
		}
		return false;
	}
	else if (ext::startsWith(cmd, "config")) {
		Config::Edit(fm);
	}
	else if (ext::startsWith(cmd, "help")) {
		Console cns(2, 4);
		cns.Add("backup\tbacks up all paths")
			.Add("\t-force forces full backup")
			.Add("restore <-f/-d> <path>\tsupports relative and absolute paths (-f file, -d directory)")
			.Add("clear\tclears all backups")
			.Add("config\tto access configurations")
			.Add("exit\tto close the app")
			.Print(false);
	}
	else if (ext::startsWith(cmd, "test")) {
		fm.PrintContent(20);
	}
	else {
		std::cout << cmd << " is not a command. Type 'help' for list of commands" << std::endl << std::endl;
	}

	std::cout << std::endl;
	return true;
}

int main() {
	Console::Clear();
	Config::Initialize();

	while (ResolveInput()) {}
	return 0;
}