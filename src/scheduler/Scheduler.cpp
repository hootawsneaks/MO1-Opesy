#include "Scheduler.h"

void enqueue(Scheduler& sched, Process& p) {
	std::unique_lock<std::mutex> lock(sched.readyMutex);
	sched.readyQueue.push(&p);
}

/**
* returns -1 if it wasn't successful, but returns 1 if was successfully set
*/
int setSchedulerConfig(Scheduler& sched, Config& config) {
	if (config.scheduler == "\"fcfs\"") sched.schedulerType = FCFS;
	else if (config.scheduler == "\"rr\"") sched.schedulerType = RR;
	else return -1;

	sched.quantumCycles = config.quantumCycles;
	sched.delaysPerExec = config.delaysPerExec;

	return 1;
}