#include "stdafx.h"
#include "File.h"
#include "Console.h"
#include "Config.h"

/*void GenerateHelpLine(const char* command, const char* description) {
	std::cout << command << "\t\t" << description << std::endl;
}*/

Config cfg;

/// <summary>Handles command recognition from user input
/// </summary>
bool ResolveInput() {
	std::cout << "What do you want to do?" << std::endl;

	std::string s;
	std::string sLower;
	std::cin >> s;

	Console::Clear();

	s = ext::trim(s);
	sLower = s;
	ext::tolower(sLower);

	if (ext::startsWith(sLower, "backup")) {
		s = s.substr(0, 6);
	}
	else if (ext::startsWith(sLower, "exit")) {
		if (s != "exit") {
			std::cout << "Are you sure you want to exit? (yes/NO)" << std::endl;
			while (true) {
				std::cin >> s;
				ext::tolower(s);
				if (ext::startsWith(s, "yes"))
					return false;
				else if (ext::startsWith(s, "no")) {
					std::cout << std::endl;
					return true;
				}

				std::cout << s << " is not a valid answer. Yes or no?" << std::endl;
			}
		}
		return false;
	}
	else if (ext::startsWith(sLower, "config")) {
		cfg.Edit();
	}
	else if (ext::startsWith(sLower, "help")) {
		Console cns(2, 4);
		cns.AddLine("backup <path>\tsupports relative and absolute paths");
		cns.AddLine("restore <path>\tsupports relative and absolute paths");
		cns.AddLine("restore all (<path>)\trestores all files to their default or set paths");
		cns.AddLine("remove all/<path>\tremoves all or set path from backup");
		cns.AddLine("exit\tto close the app");
		cns.Print();
	}
	else {
		std::cout << s << " is not a command. Type 'help' for list of commands" << std::endl << std::endl;
	}

	std::cout << std::endl;
	return true;
}

int main() {
	while (ResolveInput()) {}
	return 0;
}

