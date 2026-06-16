#include "Config.h"
#include <filesystem>
#include <fstream>
#include <iostream>

std::optional<Config> initConfig(char* argv[]) {
	std::filesystem::path exeDir = std::filesystem::path(argv[0]).parent_path();
	std::filesystem::path configPath = exeDir / "config.txt";
	Config config = {};
	std::string key, value;
	std::cout << "Initializing config file\n";
	std::ifstream file(configPath);
	if (file) {

		while (file >> key >> value) {
			if (key == "num-cpu") config.numCpu = std::stoi(value);
			if (key == "scheduler") config.scheduler = value;
			if (key == "quantum-cycles") config.quantumCycles = std::stoul(value);
			if (key == "batch-process-freq") config.batchProcessFreq = std::stoul(value);
			if (key == "min-ins") config.minIns = std::stoul(value);
			if (key == "max-ins") config.maxIns = std::stoul(value);
			if (key == "delays-per-exec") config.delaysPerExec = std::stoul(value);
		}

		std::cout << "Initialization success!\n";
		return config;

	}
	else { std::cout << "file not found" << std::endl; return std::nullopt; }
}