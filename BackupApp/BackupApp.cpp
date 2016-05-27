#include "stdafx.h"
#include "File.h"

void GenerateHelpLine(const char* command, const char* description) {
	std::cout << command << "\t\t" << description << std::endl;
}

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

	}
	else if (Ext::startsWith(sLower, "exit")) {
		if (s != "exit") {
			std::cout << "Are you sure you want to exit? (yes/NO)" << std::endl;
			while (true) {
				std::cin >> s;
				Ext::tolower(s);
				if (Ext::startsWith(s, "yes"))
					return false;
				else if (Ext::startsWith(s, "no"))
					return true;

				std::cout << s << " is not a valid answer. Yes or no?" << std::endl;
			}
		}
		return false;
	}
	else if (Ext::startsWith(sLower, "help")) {
		GenerateHelpLine("backup <path>", "supports relative and absolute paths");
		GenerateHelpLine("restore <path>", "supports relative and absolute paths");
		GenerateHelpLine("restore all (<path>)", "restores all files to their default or set paths");
		GenerateHelpLine("remove all/<path>", "removes all or set path from backup");
		GenerateHelpLine("exit", "to close the app");
		std::cout << std::endl;
	}
	else {
		std::cout << s << " is not a command. Type 'help' for list of commands" << std::endl << std::endl;
	}
	return true;
}

int main() {
	while (ResolveInput()) {}
	return 0;
}

