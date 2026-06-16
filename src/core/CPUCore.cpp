#include "CPUCore.h"

std::atomic<uint64_t> cpuTicks{ 0 };
std::atomic<bool> running{ true };

void tickLoop() {
	while (running) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		cpuTicks++;
	}
}

void startWorker(int coreId, Scheduler& sched){
	// initialize a null pointer for future use
	Process* p = nullptr;
	// grab a process if exists
	// use a scope block to remove mutex lock after
	{
		std::unique_lock<std::mutex> lock(sched.readyMutex);
		sched.cv.wait(
			lock, [&] {return !sched.readyQueue.empty() || !running;
		});

		if (!running) return;
		p = sched.readyQueue.front();
		sched.readyQueue.pop();

	}

	p->state = ProcessState::RUNNING;
	p->assignedCore = coreId;

	while (p->programCounter < p->instructions.size()) {
		executeInstruction(*p);
	}

	p->state = ProcessState::FINISHED;
	finishProcessLog(*p);

	{
		std::unique_lock<std::mutex> lock(sched.finishedMutex);
		sched.finishedList.push_back(p);
	}
}

void stopWorker() {

}