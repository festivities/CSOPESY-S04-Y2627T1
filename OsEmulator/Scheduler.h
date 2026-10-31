// CSOPESY S04-Y2627T1 - GROUP 6
// This header file contains the declarations of the SchedulerConfig struct and the Scheduler class.
//

#pragma once

#include "Process.h"
#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

// settings from config.txt (the defaults match the sample config.txt)
struct SchedulerConfig {
	uint64_t numCpu = 4;
	std::string scheduler = "rr";
	uint64_t quantumCycles = 5;
	uint64_t batchProcessFreq = 1;
	uint64_t minIns = 1000;
	uint64_t maxIns = 2000;
	uint64_t delayPerExec = 0;
};

// tick based CPU scheduler. One thread runs a tick loop that drives every core.
// Everything is guarded by one mutex and the scheduler thread never prints.
class Scheduler {
private:
	// settings (set by start)
	SchedulerConfig config;
	// CPU tick counter
	uint64_t tick;
	// ticks counted while generating (a process is made every batch-process-freq ticks)
	uint64_t genCounter;
	// true between scheduler-start and scheduler-stop
	bool generating;
	// number used for the next generated name (p01, p02, ...)
	int nextNameNumber;

	// every process ever created (name lookup and ids)
	std::vector<std::shared_ptr<Process>> all;
	// FIFO ready queue of processes waiting for a core
	// https://en.cppreference.com/w/cpp/container/queue
	std::queue<std::shared_ptr<Process>> readyQueue;
	// process on each core (nullptr when the core is idle)
	std::vector<std::shared_ptr<Process>> cores;
	// processes waiting for their wake-up tick (SLEEP)
	std::vector<std::shared_ptr<Process>> sleeping;
	// processes that finished, in completion order
	std::vector<std::shared_ptr<Process>> finished;

	// guards everything above
	// https://en.cppreference.com/w/cpp/thread/mutex
	std::mutex mtx;
	// running flag of the tick loop
	// https://en.cppreference.com/w/cpp/atomic/atomic
	std::atomic<bool> running;
	// thread that runs the tick loop
	// https://en.cppreference.com/w/cpp/thread/thread
	std::thread schedulerThread;

	// tick loop (runs on schedulerThread)
	void run();
	// helpers that expect mtx to be held
	std::shared_ptr<Process> makeProcess(const std::string& name);
	std::shared_ptr<Process> findProcess(const std::string& name);
	std::string nextAutoName();

public:
	// constructor
	Scheduler();

	// destructor (since threads are involved)
	~Scheduler();

	// non-copyable (avoid copying threads)
	Scheduler(const Scheduler&) = delete;
	Scheduler& operator=(const Scheduler&) = delete;

	// non-movable (avoid moving threads)
	Scheduler(Scheduler&&) = delete;
	Scheduler& operator=(Scheduler&&) = delete;

	// control
	void start(const SchedulerConfig& cfg);
	void stop();

	// scheduler-start / scheduler-stop. Return false if nothing changed
	bool startGenerating();
	bool stopGenerating();

	// screen -s: nullptr if a process with this name is still running
	std::shared_ptr<Process> addNamedProcess(const std::string& name);
	// screen -r: nullptr if there is no such process or it finished
	std::shared_ptr<Process> findAttachable(const std::string& name);

	// text for process-smi
	std::string processSmi(const std::shared_ptr<Process>& proc);

	// text for screen -ls and report-util
	std::string report();
};
