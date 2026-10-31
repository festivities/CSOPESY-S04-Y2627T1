// CSOPESY S04-Y2627T1 - GROUP 6
// This header file contains the declarations of the Process and Scheduler classes.
//

#pragma once

#include <atomic>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

// a process that prints a fixed number of instructions to its own log file
class Process {
private:
	// process name, also the log file name (<name>.txt)
	std::string name;
	// creation time formatted as %m/%d/%Y %I:%M:%S%p
	std::string created;
	// number of instructions the process has to run
	int total;

	// instructions executed so far (written by a core thread, read by screen -ls)
	// https://en.cppreference.com/w/cpp/atomic/atomic
	std::atomic<int> executed;

public:
	// constructor (creates the log file with its header)
	Process(const std::string& name, int total);

	// execute one print instruction on the given core (appends one line to the log)
	void print(int core);

	// getters
	std::string getName() const;
	std::string getCreated() const;
	int getTotal() const;
	int getExecuted() const;
};

// first come first serve scheduler: one scheduler thread and one worker thread per core
class Scheduler {
private:
	// FIFO ready queue of processes waiting for a core
	// https://en.cppreference.com/w/cpp/container/queue
	std::queue<std::shared_ptr<Process>> readyQueue;
	// process running on each core (nullptr when the core is idle)
	std::vector<std::shared_ptr<Process>> cores;
	// processes that finished, in completion order
	std::vector<std::shared_ptr<Process>> finished;
	// guards readyQueue, cores and finished
	// https://en.cppreference.com/w/cpp/thread/mutex
	std::mutex mtx;

	// atomic boolean to control running state of the scheduler and cores
	std::atomic<bool> running;

	// threads: scheduler thread and one worker thread per core
	// https://en.cppreference.com/w/cpp/thread/thread
	std::thread schedulerThread;
	std::vector<std::thread> coreThreads;

	// loops run on separate threads
	void schedule();
	void runCore(int core);

public:
	// constructor (numCores = number of cores)
	Scheduler(int numCores);

	// destructor (since threads are involved)
	~Scheduler();

	// non-copyable (avoid copying threads)
	Scheduler(const Scheduler&) = delete;
	Scheduler& operator=(const Scheduler&) = delete;

	// non-movable (avoid moving threads)
	Scheduler(Scheduler&&) = delete;
	Scheduler& operator=(Scheduler&&) = delete;

	// add a new process to the back of the ready queue
	void addProcess(const std::string& name, int instructions);

	// control
	void start();
	void stop();

	// print the running and finished processes
	void screenList();
};
