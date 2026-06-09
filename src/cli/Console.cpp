#ifdef _WIN32
#include <windows.h> // heh yeah im doing that cross compatibility thing. try to make this as platform agnostic as possible.
#endif
#include "Console.h"
#include "core/Config.h"
#include <unordered_map>
#include <functional> // for lambdas

void console(char* argv[]) {
	std::string input;
	std::optional<Config> config;
	bool initialized = false;
	bool running = true;
	std::unordered_map<std::string, std::function<void()>> commands = {
		{"initialize", [&]() {
			auto config = init(argv);
			if (!config) { std::cout << "Initialization failed." << std::endl; return; }
			else { initialized = true; }
		}},
		{"exit", [&]() {
			running = false;
		}},
		{"screen", [&]() {
			if (initialized) {
				/*screen implementation*/
			}
		}},
	};

#ifdef _WIN32															// for windows theres a chance that the terminal doesnt 
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);						// support ascii escape characters (apparently), this is assurance
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
		std::getline(std::cin, input);
		auto it = commands.find(input);
		if (it != commands.end()) {
			it->second();
		}
	} while (running);
}