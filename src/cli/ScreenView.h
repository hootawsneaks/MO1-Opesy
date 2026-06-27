#pragma once

#include "process/Process.h"
#include "scheduler/Scheduler.h"
#include <string>
#include <cstdint>
#include <unordered_map>
#include <mutex>
extern std::unordered_map<std::string, Process*> g_processMap;
extern std::mutex g_processMapMutex;

// Creates a dummy process, adds it to the global map, and queues it in the scheduler.
Process* createProcess(const std::string& name, Scheduler& sched, uint32_t minIns, uint32_t maxIns);

// Retrieves a process pointer by name. Returns nullptr if not found.
Process* getProcess(const std::string& name);

// Hijacks the console thread for the interactive process screen.
void openProcessScreen(Process* p);