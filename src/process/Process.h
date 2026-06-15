#pragma once
#include <cstdint>
#include <string>
#include <fstream>
#include <vector>

enum InstructionType { PRINT, DECLARE, ADD, SUBTRACT, SLEEP, FOR };
enum class ProcessState {
	READY,
	RUNNING,
	WAITING,
	FINISHED
};

struct Instruction {
	InstructionType type;
	std::string arg1;
};

struct Process {
	std::string processName;
	uint16_t pid;
	std::vector<Instruction> instructions;
	uint16_t programCounter;
	ProcessState state;
	int assignedCore{ -1 };
	std::string timestamp;
	std::ofstream outputFile;
};

void executeInstruction(Process& p);
void initProcessLog(Process& p);
void finishProcessLog(Process& p);
std::string getTimestamp();