#include "Process.h"
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <algorithm>  // std::clamp
//#include <thread>

// voodoo grabbed from the netz. how the hell is it so difficult to get the damn time??
std::string getTimestamp() { 
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    struct tm timeInfo;
    localtime_s(&timeInfo, &time);

    std::stringstream ss;
    ss << std::put_time(&timeInfo, "(%m/%d/%Y %I:%M:%S%p)");
    return ss.str();
}

// Helper: resolve a token to a uint16_t value.
static uint16_t resolve(Process& p, const std::string& token) {
    auto it = p.symbolTable.find(token);
    if (it != p.symbolTable.end()) {
        return it->second;
    }
    try {
        int raw = std::stoi(token);
        return static_cast<uint16_t>(std::clamp(raw, 0, 65535));
    }
    catch (...) {
        return 0;
    }
}

void executeInstruction(Process& p) {
    if (p.programCounter >= p.instructions.size()) return;

    Instruction& ins = p.instructions[p.programCounter];

    switch (ins.type) {

        // ------------------------------------------------------------------
    case PRINT: {
        std::string msg = ins.arg1.empty()
            ? ("Hello world from " + p.processName + "!")
            : ins.arg1;

        p.outputFile << getTimestamp()
            << " Core: " << p.assignedCore
            << " \"" << msg << "\""
            << std::endl;
        break;
    }

              // ------------------------------------------------------------------
    case DECLARE: {
        // Symbol table is ONLY written to here (and ADD/SUB for their dest).
        if (!ins.arg1.empty()) {
            uint16_t val = resolve(p, ins.arg2);
            p.symbolTable[ins.arg1] = val;
        }
        break;
    }

                // ------------------------------------------------------------------
    case ADD: {
        // var1 = var2/value + var3/value
        if (ins.arg1.empty()) break;

        uint16_t v2 = resolve(p, ins.arg2);
        uint16_t v3 = resolve(p, ins.arg3);

        // Clamp result to [0, 65535] per spec
        uint32_t raw = static_cast<uint32_t>(v2) + static_cast<uint32_t>(v3);
        uint16_t result = static_cast<uint16_t>(std::min(raw, static_cast<uint32_t>(65535)));

        p.symbolTable[ins.arg1] = result;
        break;
    }

            // ------------------------------------------------------------------
    case SUBTRACT: {
        // var1 = var2/value - var3/value
        if (ins.arg1.empty()) break;

        uint16_t v2 = resolve(p, ins.arg2);
        uint16_t v3 = resolve(p, ins.arg3);

        // Floor at 0 (uint16 can't go negative)
        uint16_t result = (v2 > v3) ? static_cast<uint16_t>(v2 - v3) : 0;

        p.symbolTable[ins.arg1] = result;
        break;
    }

                 // ------------------------------------------------------------------
    case SLEEP: {
        // arg1 = number of CPU ticks to sleep (uint8 range: 0-255)
        // Sets sleepTicks on the process; CPUCore::startWorker is
        // responsible for spinning until sleepTicks reaches 0.
        if (!ins.arg1.empty()) {
            try {
                int raw = std::stoi(ins.arg1);
                p.sleepTicks = static_cast<uint8_t>(std::clamp(raw, 0, 255));
            }
            catch (...) {
                p.sleepTicks = 0;
            }
        }
        break;
    }

              // ------------------------------------------------------------------
    case FOR:
        // FOR loops are unrolled at process-creation time into a flat
        // instruction list, so this case should never be reached.
        break;

    default:
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

