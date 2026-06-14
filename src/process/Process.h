#pragma once

struct Process {
	enum class ProcessState {
		READY,
		RUNNING,
		WAITING,
		FINISHED
	};
};