#include "stdafx.h"
#include "File.h"
#include "Console.h"

/*void GenerateHelpLine(const char* command, const char* description) {
	std::cout << command << "\t\t" << description << std::endl;
}*/

/// <summary>Handles command recognition from user input
/// </summary>
bool ResolveInput() {
	std::cout << "What do you want to do?" << std::endl;

	std::string s;
	std::string sLower;
	std::cin >> s;

#ifdef _WIN32
	std::system("cls");
#elif __linux__
	std::system("clear");
#endif

	s = Ext::trim(s);
	sLower = s;
	Ext::tolower(sLower);

	if (Ext::startsWith(sLower, "backup")) {
		s = s.substr(0, 6);
	}
	else if (Ext::startsWith(sLower, "exit")) {
		if (s != "exit") {
			std::cout << "Are you sure you want to exit? (yes/NO)" << std::endl;
			while (true) {
				std::cin >> s;
				Ext::tolower(s);
				if (Ext::startsWith(s, "yes"))
					return false;
				else if (Ext::startsWith(s, "no")) {
					std::cout << std::endl;
					return true;
				}

				std::cout << s << " is not a valid answer. Yes or no?" << std::endl;
			}
		}
		return false;
	}
	else if (Ext::startsWith(sLower, "help")) {
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

