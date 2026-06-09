#include "Config.h"

bool init(char* argv[]) {
	std::cout << std::filesystem::current_path() << "\n";
	std::filesystem::path exeDir = std::filesystem::path(argv[0]).parent_path();
	std::filesystem::path configPath = exeDir / "config.txt";
	std::ifstream file(configPath);
	if (file) {
		std::cout << "Config file successfully found.\n";
		return 1;
	}
	else { std::cout << "file not found\n"; return 0; }
}