#pragma once
#include <queue>
#include <mutex>
#include <vector>
#include <condition_variable>
#include "process/Process.h"
#include "core/config.h"

enum SchedulerType { FCFS, RR };

struct Scheduler {
	std::queue<Process*> readyQueue;
	std::vector<Process*> finishedList;
	std::vector<Process*> coreProcesses;
	std::vector<std::thread> coreThread; 
	std::mutex readyMutex;
	std::mutex coreProcessesMutex;
	std::mutex finishedMutex;
	std::condition_variable cv;
	SchedulerType schedulerType;
	int quantumCycles;
	int delaysPerExec;
};

void enqueue(Scheduler& sched, Process& p);

int setSchedulerConfig(Scheduler& sched, Config& config);