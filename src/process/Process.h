#pragma once
#include <cstdint>
#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>

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
	std::string arg2;
	std::string arg3;
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
	std::unordered_map<std::string, uint16_t> symbolTable; // written to on DECLARE
	uint8_t sleepTicks{ 0 }; //remaining ticks to sleep for SLEEP instruction
};

void executeInstruction(Process& p);
void initProcessLog(Process& p);
void finishProcessLog(Process& p);
std::string getTimestamp();