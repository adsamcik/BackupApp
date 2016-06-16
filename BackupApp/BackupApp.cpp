#include "stdafx.h"
#include "Console.h"
#include "Config.h"
#include "FileManager.h"

using std::string;

FileManager fm;

/// <summary>Handles command recognition from user input
/// </summary>
bool ResolveInput() {
	std::cout << "::::";

	std::vector<string> input;
	string s;
	std::getline(std::cin, s);
	std::istringstream tss(s);
	while (tss >> s)
		input.push_back(s);

	string cmd = ext::tolower(input[0]);

	if (ext::startsWith(cmd, "backup")) {
		if (input.size() > 1 && input[1] == "-force")
			std::cout << "force" << std::endl;
		fm.BackupAll();
	}
	else if (ext::startsWith(cmd, "restore")) {
		if (input.size() > 1)
			fm.Restore(input[1]);
		//fm.BackupAll();
	}
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
		cns.AddLine("backup <path>\tsupports relative and absolute paths");
		cns.AddLine("\t-force forces full backup");
		cns.AddLine("restore <path>\tsupports relative and absolute paths");
		cns.AddLine("restore all (<path>)\trestores all files to their default or set paths");
		cns.AddLine("remove all/<path>\tremoves all or set path from backup");
		cns.AddLine("exit\tto close the app");
		cns.Print();
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