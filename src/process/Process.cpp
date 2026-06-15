#include "Process.h"
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <iostream>

// voodoo grabbed from the netz. how the hell is it so difficult to get the damn time??
std::string getTimestamp() { 
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    struct tm timeInfo;
    localtime_s(&timeInfo, &time);

    std::stringstream ss;
    ss << std::put_time(&timeInfo, "%m/%d/%Y %I:%M:%S%p");
    return ss.str();
}

void executeInstruction(Process& p) {
    Instruction& ins = p.instructions[p.programCounter];

    switch (ins.type) {
        case PRINT:
            p.outputFile << getTimestamp() << "Core: " << p.assignedCore << "Hello world from " << p.processName << "!" << std::endl;
            break;
    }
    p.programCounter++;
}

void initProcessLog(Process &p) {
    p.outputFile.open(p.processName + ".txt");
    p.outputFile << "Process name: " << p.processName << std::endl;
    p.outputFile << "Logs:" << std::endl;
    p.outputFile << std::endl;
}

void finishProcessLog(Process& p) {
    p.outputFile.close();
}

