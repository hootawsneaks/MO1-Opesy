#include "Console.h"
#include <windows.h>

void console() {
	std::string input;
#ifdef _WIN32															// for windows theres a chance that the terminal doesnt support ascii escape characters (apparently), this is assurance
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD mode = 0;
	GetConsoleMode(hOut, &mode);
	SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
	std::cout << "\033[32m" << logo << "\033[0m\n";
	std::cout << "-------------------------------------------------------" << "\n";
	std::cout << "\033[32m" << "Hello! Welcome to the CSOPESY command line!" << "\033[0m\n\n";
	std::cout << "\033[33m" << "Developers: \nKean Carvin, Gideon Chua, Daniel Pua, Bea Uy" << "\033[0m\n";
	std::cout << "-------------------------------------------------------" << "\n";
	do {
		std::cout << "> ";
		std::cin >> input;
	} while (input != "exit");
}