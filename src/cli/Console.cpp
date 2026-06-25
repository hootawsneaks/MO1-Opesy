#ifdef _WIN32
#include <windows.h> // heh yeah im doing that cross compatibility thing. try to make this as platform agnostic as possible.
#endif
#include "Console.h"
#include "core/Config.h"
#include <iostream>
#include <unordered_map>
#include <functional> // for lambdas
#include <sstream> // for convenient text extraction

void printProcessList(Scheduler& sched) {

	int coresUsed = 0;
	{
		std::lock_guard<std::mutex> lock(sched.coreProcessesMutex);
		for (auto* p : sched.coreProcesses) {
			if (p != nullptr) coresUsed++;
		}
	}

	std::cout << "CPU utilization: " << (float)coresUsed / sched.coreProcesses.size() * 100 << "\n";
	std::cout << "Cores used: " << coresUsed << "\n";
	std::cout << "Cores available: " << sched.coreProcesses.size() - coresUsed << "\n";
	std::cout << "---------------------------------\n";
	std::cout << "Running processes:\n";
	{
		std::unique_lock<std::mutex> lock(sched.coreProcessesMutex);
		for (int i = 0; i < sched.coreProcesses.size(); i++) {
			if (sched.coreProcesses[i] != nullptr) {
				std::cout << sched.coreProcesses[i]->processName << " " << sched.coreProcesses[i]->timestamp << " " << "Core: " << sched.coreProcesses[i]->assignedCore;
				std::cout << " " << sched.coreProcesses[i]->programCounter << " / " << sched.coreProcesses[i]->instructions.size() << std::endl;
			}
		}
	}
	std::cout << "Finished processes:\n";
	{
		std::unique_lock<std::mutex> lock(sched.finishedMutex);
		for (int i = 0; i < sched.finishedList.size(); i++) {
			std::cout << sched.finishedList[i]->processName << " " << sched.finishedList[i]->timestamp << " Finished ";
			std::cout << sched.finishedList[i]->programCounter << " / " << sched.finishedList[i]->instructions.size() << std::endl;
		}
	}
	std::cout << "---------------------------------" << std::endl;
}

void console(char* argv[]) {
	std::string input;
	Config config;
	std::thread tickThread;
	Scheduler sched;

	bool initialized = false; // a boolean corresponding to if the init function has ran (successfully)
	std::unordered_map<std::string, std::function<void()>> commands = {
		{"initialize", [&]() {
			if (!initialized) {
				std::optional<Config> result = initConfig(argv);
				if (!result) { std::cout << "Initialization failed." << std::endl; return; }
				Config config = *result;
				sched.coreProcesses.resize(config.numCpu, nullptr);
				startCores(config.numCpu, sched);
				tickThread = std::thread(tickLoop);
				initialized = true;

				// set scheduler values
				if (setSchedulerConfig(sched, config) == -1) std::cout << "Invalid scheduler selected -- " << config.scheduler << "\n";

			}
		}},
		{"exit", [&]() {
			if (initialized) {
				stopCores(sched);
				tickThread.join();
			}
			running = false;
		}},
		{"screen", [&]() {
			if (!initialized) { std::cout << "Run initialize first!\n"; return; }
			std::string _, flag, arg;
			std::istringstream ss(input);
			ss >> _ >> flag >> arg;
			std::cout << flag << "\n";
			if (flag == "-ls") {
				/* list processes */
				printProcessList(sched);
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
		}},
		{"print", [&]() {
			print(sched);
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
	} while (running);

}