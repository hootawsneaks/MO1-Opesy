#pragma once
#include <atomic>
#include <thread>
// hmmm...?

extern std::atomic<uint64_t> cpuTicks; // number of ticks the cpu has gone thru.
extern std::atomic<bool> running; // state of os-- running implies that program has initialized successfully.

void tickLoop(); // function handling adding stuff to cpuTicks. Should run in a separate thread so it just keeps going. dependent on 'running' variable.
void startWorker();
void stopWorker();

// so what should be the design?