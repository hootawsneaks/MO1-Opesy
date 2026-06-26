#ifdef _WIN32
#include <windows.h> // heh yeah im doing that cross compatibility thing. try to make this as platform agnostic as possible.
#endif
#include "Console.h"
#include "core/Config.h"
#include "ScreenView.h"
#include <fstream>
#include <filesystem>
#include <iomanip>
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

void generateReportUtil(Scheduler& sched) {
	std::string fileName = "csopesy-log.txt";
	std::ofstream log(fileName);
	if (!log.is_open()) {
		std::cout << "Failed to open " << fileName << " for writing.\n";
		return;
	}

	int coresUsed = 0;
	{
		std::lock_guard<std::mutex> lock(sched.coreProcessesMutex);
		for (auto* p : sched.coreProcesses) {
			if (p != nullptr) coresUsed++;
		}
	}

	log << "CPU utilization: " << (float)coresUsed / sched.coreProcesses.size() * 100 << "%\n";
	log << "Cores used: " << coresUsed << "\n";
	log << "Cores available: " << sched.coreProcesses.size() - coresUsed << "\n";
	log << "---------------------------------\n";
	log << "Running processes:\n";
	{
		std::unique_lock<std::mutex> lock(sched.coreProcessesMutex);
		for (int i = 0; i < sched.coreProcesses.size(); i++) {
			if (sched.coreProcesses[i] != nullptr) {
				log << sched.coreProcesses[i]->processName << "\t" 
					<< sched.coreProcesses[i]->timestamp << "\t" 
					<< "Core: " << sched.coreProcesses[i]->assignedCore << "\t" 
					<< sched.coreProcesses[i]->programCounter << " / " 
					<< sched.coreProcesses[i]->instructions.size() << "\n";
			}
		}
	}
	log << "\nFinished processes:\n";
	{
		std::unique_lock<std::mutex> lock(sched.finishedMutex);
		for (int i = 0; i < sched.finishedList.size(); i++) {
			log << sched.finishedList[i]->processName << "\t" 
				<< sched.finishedList[i]->timestamp << "\tFinished\t" 
				<< sched.finishedList[i]->programCounter << " / " 
				<< sched.finishedList[i]->instructions.size() << "\n";
		}
	}
	log << "---------------------------------\n";
	log.close();

	std::filesystem::path currentPath = std::filesystem::current_path() / fileName;
	std::cout << "Report generated at " << currentPath.string() << "!\n";
}

void console(char* argv[]) {
	std::string input;
	Config config;
	std::thread tickThread;
	Scheduler sched;
	std::atomic<bool> schedulerRunning{ false };
	std::thread backgroundSchedulerThread;
	uint32_t processCounter = 1;

	bool initialized = false; // a boolean corresponding to if the init function has ran (successfully)
	std::unordered_map<std::string, std::function<void()>> commands = {
		{"initialize", [&]() {
			if (!initialized) {
				std::optional<Config> result = initConfig(argv);
				if (!result) { std::cout << "Initialization failed." << std::endl; return; }
				config = *result;
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

				// Safely shut down the generator thread if it was left running
				schedulerRunning = false;
				if (backgroundSchedulerThread.joinable()) {
					backgroundSchedulerThread.join();
				}
			}
			running = false;
		}},
		{"screen", [&]() {
			if (!initialized) { std::cout << "Run initialize first!\n"; return; }
			
			std::string _, flag, arg;
			std::istringstream ss(input);
			ss >> _ >> flag >> arg;

			if (flag == "-ls") {
				printProcessList(sched);
			}
			else if (flag == "-s") {
				if (arg.empty()) {
					std::cout << "Process name required.\n";
					return;
				}
				Process* p = getProcess(arg);
				if (p != nullptr) {
					std::cout << "Process " << arg << " already exists.\n";
				} else {
					p = createProcess(arg, sched, config.minIns, config.maxIns);
					openProcessScreen(p);
					// Reprint main header to restore standard UI feel
					std::cout << "\033[32m" << logo << "\033[0m\n";
				}
			}
			else if (flag == "-r") {
				if (arg.empty()) {
					std::cout << "Process name required.\n";
					return;
				}
				Process* p = getProcess(arg);
				if (p == nullptr) {
					std::cout << "Process " << arg << " not found.\n";
				} else {
					openProcessScreen(p);
					// Reprint main header to restore standard UI feel
					std::cout << "\033[32m" << logo << "\033[0m\n";
				}
			}
			else { std::cout << "Screen flag not recognized.\n"; }
		}},
		{"scheduler-start", [&]() {
			if (!initialized) { std::cout << "Run initialize first!\n"; return; }
			if (schedulerRunning) { std::cout << "Scheduler is already generating processes.\n"; return; }
			
			schedulerRunning = true;
			backgroundSchedulerThread = std::thread([&]() {
				uint64_t lastTick = cpuTicks.load();
				while (schedulerRunning && running) {
					// Check if enough CPU ticks have passed based on config.txt
					if (cpuTicks.load() - lastTick >= config.batchProcessFreq) {
						
						// Format name to zero-pad (e.g. p01, p02... p1240)
						std::stringstream ss;
						ss << "p" << std::setfill('0') << std::setw(2) << processCounter++;
						
						createProcess(ss.str(), sched, config.minIns, config.maxIns);
						lastTick = cpuTicks.load();
					} else {
						// Yield to prevent pegging the CPU at 100% while waiting
						std::this_thread::yield();
					}
				}
			});
			std::cout << "Dummy process generation started.\n";
		}},
		{"scheduler-stop", [&]() {
			if (!initialized) { std::cout << "Run initialize first!\n"; return; }
			if (!schedulerRunning) { std::cout << "Scheduler is not currently running.\n"; return; }
			
			schedulerRunning = false;
			if (backgroundSchedulerThread.joinable()) {
				backgroundSchedulerThread.join();
			}
			std::cout << "Dummy process generation stopped.\n";
		}},
		{"report-util", [&]() {
			if (!initialized) { std::cout << "Run initialize first!\n"; return; }
			generateReportUtil(sched);
		}},
		{"ticks", [&]() {
			std::cout << cpuTicks.load() << "\n";
			return;
		}},
		{"print", [&]() {
			print(sched);
		}},
		{"print20", [&]() {
			for (int i = 0; i < 20; i++) {
				print(sched);
			}
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