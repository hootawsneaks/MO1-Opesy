#pragma once
#include <atomic>
#include <thread>
#include  "scheduler/Scheduler.h"
// hmmm...?

extern std::atomic<uint64_t> cpuTicks; // number of ticks the cpu has gone thru.
extern std::atomic<bool> running; // state of os-- running implies that program has initialized successfully.
extern std::atomic<uint16_t> nextPid;

void tickLoop(); // function handling adding stuff to cpuTicks. Should run in a separate thread so it just keeps going. dependent on 'running' variable.
void startWorker(int coreId, Scheduler& sched); // job hunting. now i get why theyre called workers
void startCores(int numCpu, Scheduler& sched);
void stopCores(Scheduler& sched);
