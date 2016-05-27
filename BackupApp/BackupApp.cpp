#include "stdafx.h"
#include "File.h"

void GenerateHelpLine(const char* command, const char* description) {
	std::cout << command << "\t\t" << description << std::endl;
}

bool ResolveInput() {
	std::cout << "What do you want to do?" << std::endl;

	std::string s;
	std::cin >> s;

#ifdef _WIN32
	std::system("cls");
#elif __linux__
	std::system("clear");
#endif

	s = Ext::trim(s);
	if (Ext::startsWith(s, "backup")) {
		
	}
	else if (Ext::startsWith(s, "help")) {
		GenerateHelpLine("backup <path>", "supports relative and absolute paths");
		GenerateHelpLine("restore <path>", "supports relative and absolute paths");
		GenerateHelpLine("restore all (<path>)", "restores all files to their default or set paths");
		GenerateHelpLine("remove all/<path>", "removes all or set path from backup");
		GenerateHelpLine("exit", "to close the app");
	}
	else {
		std::cout << s << " is not a command. Type 'help' for list of commands" << std::endl << std::endl;
	}
	return true;
}

int main() {
	while (ResolveInput()) {
	}
	return 0;
}

