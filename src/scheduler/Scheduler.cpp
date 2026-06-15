#include "Scheduler.h"

void enqueue(Scheduler& sched, Process& p) {
	std::unique_lock<std::mutex> lock(sched.readyMutex);
	sched.readyQueue.push(&p);
}