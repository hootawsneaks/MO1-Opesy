#include "CPUCore.h"

std::atomic<uint64_t> cpuTicks{ 0 };
std::atomic<bool> running{ true };
std::atomic<uint16_t> nextPid{ 0 };

void tickLoop() {
	while (running) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		cpuTicks++;
	}
}

void startWorker(int coreId, Scheduler& sched){
	while (running) {
		// initialize a null pointer for future use
		Process* p = nullptr;
		// grab a process if exists
		// use a scope block to remove mutex lock after
		uint64_t lastTick = cpuTicks.load() - 1;

		{
			std::unique_lock<std::mutex> lock(sched.readyMutex);
			sched.cv.wait(
				lock, [&] {return !sched.readyQueue.empty() || !running;
				});

			if (!running) break;
			p = sched.readyQueue.front();
			sched.readyQueue.pop();

		}

		p->state = ProcessState::RUNNING;
		p->assignedCore = coreId;
		{
			std::unique_lock<std::mutex> lock(sched.coreProcessesMutex);
			sched.coreProcesses[coreId] = p;
		}

		initProcessLog(*p);
		while (p->programCounter < p->instructions.size()) {
			while (cpuTicks.load() == lastTick && running) {
				std::this_thread::yield();
			}
			lastTick = cpuTicks.load();
			executeInstruction(*p);
		}

		p->state = ProcessState::FINISHED;
		{
			std::unique_lock<std::mutex> lock(sched.coreProcessesMutex);
			sched.coreProcesses[coreId] = nullptr;
		}
		finishProcessLog(*p);

		{
			std::unique_lock<std::mutex> lock(sched.finishedMutex);
			sched.finishedList.push_back(p);
		}
	}
}

void startCores(int numCpu, Scheduler& sched) {
	for (int i = 0; i < numCpu; i++) {
		sched.coreThread.emplace_back(startWorker, i, std::ref(sched));
	}
}

// stopCores asserts that running is finished (because why would you stop the cores otherwise) and notifies other threads to shut down.
void stopCores(Scheduler& sched) {
	running = false;
	sched.cv.notify_all();
	for (auto& t : sched.coreThread) {
		if (t.joinable()) t.join();
	}
}

void print(Scheduler& sched) {
	Process* p = new Process();
	p->processName = "process_" + std::to_string(nextPid);
	p->pid = nextPid;
	nextPid++;

	Instruction printInst = { InstructionType::PRINT, "print" };
	for (int i = 0; i < 100; i++) {
		p->instructions.push_back(printInst);
	}
	p->programCounter = 0;
	p->timestamp = getTimestamp();
	enqueue(sched, *p);
	sched.cv.notify_one();
}