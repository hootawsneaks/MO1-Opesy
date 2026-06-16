#ifdef _WIN32
#include <windows.h> // heh yeah im doing that cross compatibility thing. try to make this as platform agnostic as possible.
#endif
#include "Console.h"
#include "core/Config.h"
#include "core/CPUCore.h"
#include <iostream>
#include <unordered_map>
#include <functional> // for lambdas
#include <sstream> // for convenient text extraction

void console(char* argv[]) {
	std::string input;
	Config config;
	std::thread tickThread;
	Scheduler sched;

	bool initialized = false; // a boolean corresponding to if the init function has ran (successfully)
	std::unordered_map<std::string, std::function<void()>> commands = {
		{"initialize", [&]() {
			std::optional<Config> result = initConfig(argv);
			if (!result) { std::cout << "Initialization failed." << std::endl; return; }
			Config config = *result;
			startCores(config.numCpu, sched);
			initialized = true; 
			tickThread = std::thread(tickLoop); 
		}},
		{"exit", [&]() {
			// redundant but just to be clear. running stops here. stopCores does so.
			stopCores(sched);
			tickThread.join();
		}},
		{"screen", [&]() {
			if (!initialized) { std::cout << "Run initialize first!\n"; return; }
			std::string _, flag, arg;
			std::istringstream ss(input);
			ss >> _ >> flag >> arg;
			std::cout << flag << "\n";
			if (flag == "-ls") {
				/* list processes */
				std::cout << "im listing\n";
			}
			else if (flag == "-s") {
				/* create process where arg is the name */
				std::cout << "im creating\n";
			}
			else if (flag == "-r") {
				/* reattach to process where arg is the process */
				std::cout << "im attaching\n";
			}
			else { std::cout << "Screen flag not recognized.\n"; }
		}},
		{"scheduler-start", [&]() {
			/* nothing burger */
			if (!initialized) { std::cout << "Run initialize first!\n"; return; }
		}},
		{"scheduler-stop", [&]() {
			/* something burger */
			if (!initialized) { std::cout << "Run initialize first!\n"; return; }
		}},
		{"report-util", [&]() {
			/* something burger */
			if (!initialized) { std::cout << "Run initialize first!\n"; return; }
		}},
		{"ticks", [&]() {
			std::cout << cpuTicks.load() << "\n";
			return;
		}}
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
		std::cout << "qtsRoot:\\> ";
		std::getline(std::cin, input);
		std::istringstream ss(input);
		std::string cmd;
		ss >> cmd;
		auto it = commands.find(cmd);
		if (it != commands.end()) {
			it->second();
		}
	} while (running || !initialized);
}