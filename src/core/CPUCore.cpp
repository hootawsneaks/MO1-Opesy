#include "CPUCore.h"

std::atomic<uint64_t> cpuTicks{ 0 };
std::atomic<bool> running{ true };

void tickLoop() {
	while (running) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		cpuTicks++;
	}
}