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
		// initialize a null pointer for future use, for holding the process that the core will run.
		Process* p = nullptr;
		// grab a process if exists
		// use a scope block to remove mutex lock after
		uint64_t lastTick = cpuTicks.load() - 1;

		// grab process from ready queue
		{	
			std::unique_lock<std::mutex> lock(sched.readyMutex);
			sched.cv.wait(
				lock, [&] {return !sched.readyQueue.empty() || !running;
				});

			if (!running) break;
			p = sched.readyQueue.front();	// get the process at the front of ready queue
			sched.readyQueue.pop();			// remove process from ready queue

		}

		// claim process for this core
		p->state = ProcessState::RUNNING;
		p->assignedCore = coreId;
		{
			std::unique_lock<std::mutex> lock(sched.coreProcessesMutex);
			sched.coreProcesses[coreId] = p;		// record the process for this core
		}
		
		// if process has not run before
		if (p->programCounter == 0) initProcessLog(*p);			// open p's log file

		// initialize variables for RR for this turn
		int cyclesUsed = 0,
			delayCounter = 0;
		bool preempted = 0;

		// FCFS: run process to completion 
		while (p->programCounter < p->instructions.size()) {	// as long as has process left
			while (cpuTicks.load() == lastTick && running) {	// while clock hasn't ticked since last tick
				std::this_thread::yield();						// spin, wait for next tick
			}
			lastTick = cpuTicks.load();							// last tick = cpu tick
			executeInstruction(*p);								// advance program counter

			cyclesUsed += 1;									// add one to the cycle counter for RR

			if (delayCounter >= sched.delaysPerExec) {
				executeInstruction(*p);
				delayCounter = 0;
			}
			else {
				delayCounter += 1;
			}

			if (sched.schedulerType == RR && cyclesUsed >= sched.quantumCycles) {
				preempted = true;
				break;
			}
			
		}

		// free core's slot
		{
			std::unique_lock<std::mutex> lock(sched.coreProcessesMutex);
			sched.coreProcesses[coreId] = nullptr;
		}
		
		// if preempted and still as instructions left
		if (preempted && p->programCounter < p->instructions.size()) {
			p->state = ProcessState::READY;	// mark p as ready
			p->assignedCore = -1;	// clear p's assigned core
			enqueue(sched, *p);		// put p at the back of the ready queue
			sched.cv.notify_one();	// notify one waiting core
		}
		else {
			// process is finished
			p->state = ProcessState::FINISHED;		// mark p as finished
			{
				std::unique_lock<std::mutex> lock(sched.coreProcessesMutex);
				sched.coreProcesses[coreId] = nullptr;
			}
			finishProcessLog(*p);	// close p's logfile

			{
				std::unique_lock<std::mutex> lock(sched.finishedMutex);
				sched.finishedList.push_back(p);	// add p to finished list
			}
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