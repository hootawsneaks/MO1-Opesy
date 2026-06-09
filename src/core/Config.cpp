#include "Config.h"
#include <filesystem>
#include <fstream>
#include <iostream>

std::optional<Config> init(char* argv[]) {
	std::filesystem::path exeDir = std::filesystem::path(argv[0]).parent_path();
	std::filesystem::path configPath = exeDir / "config.txt";
	Config config = {};
	std::string key, value;
	std::cout << "Initializing config file\n";
	std::ifstream file(configPath);
	if (file) {
		std::cout << "Config file successfully found at " << configPath << "\n";

		if (file.is_open()) {
			std::cout << "Config file successfully opened\n";

			while (file >> key >> value) {
				if (key == "num-cpu") config.numCpu = std::stoi(value);
				if (key == "scheduler") config.scheduler = value;
				if (key == "quantum-cycles") config.quantumCycles = std::stoi(value);
				if (key == "batch-process-freq") config.batchProcessFreq = std::stoi(value);
				if (key == "min-ins") config.minIns = std::stoi(value);
				if (key == "max-ins") config.maxIns = std::stoi(value);
				if (key == "delays-per-exec") config.delaysPerExec = std::stoi(value);
			}

			std::cout << "Config file successfully imported\n";
			std::cout << "Initialization success!\n";
			return config;

		}
		else { std::cout << "File is corrupted or malformed." << std::endl; return std::nullopt; }

	}
	else { std::cout << "file not found" << std::endl; return std::nullopt; }
}