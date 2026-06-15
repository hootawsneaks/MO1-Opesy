#include "Process.h"
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

std::string getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%m/%d/%Y %I:%M:%S%p");
    return ss.str();
}

void executeInstruction(Process& p) {
    Instruction& ins = p.instructions[p.programCounter];

    switch (ins.type) {
        case PRINT:
            // writeLog(getTimestamp(), p.assignedCore);
            break;
    }
    p.programCounter++;
}