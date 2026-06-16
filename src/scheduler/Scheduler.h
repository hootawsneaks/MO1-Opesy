#pragma once
#include <queue>
#include <mutex>
#include <vector>
#include <condition_variable>
#include "process/Process.h"

struct Scheduler {
	std::queue<Process*> readyQueue;
	std::vector<Process*> finishedList;
	std::vector<Process*> coreProcesses;
	std::vector<std::thread> coreThread; 
	std::mutex readyMutex;
	std::mutex coreProcessesMutex;
	std::mutex finishedMutex;
	std::condition_variable cv;
};

void enqueue(Scheduler& sched, Process& p);