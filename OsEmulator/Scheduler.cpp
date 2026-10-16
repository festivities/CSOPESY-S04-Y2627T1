// CSOPESY S04-Y2627T1 - GROUP 6
// This file contains the implementation of the Process and Scheduler classes, which run
// the FCFS scheduler. Scheduler and core threads never print to the console.
//

#include "Scheduler.h"
#include "Marquee.h"
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

// Helper to get the current local time as %m/%d/%Y %I:%M:%S%p
// https://en.cppreference.com/w/cpp/io/manip/put_time
std::string timeNow() {
	std::time_t now = std::time(nullptr);
	std::tm local{};
	// https://en.cppreference.com/w/c/chrono/localtime
	localtime_s(&local, &now);
	std::ostringstream out;
	out << std::put_time(&local, "%m/%d/%Y %I:%M:%S%p");
	return out.str();
}

// Process implementation
Process::Process(const std::string& name, int total) {
	this->name = name;
	this->created = timeNow();
	this->total = total;
	this->executed = 0;

	// create (or truncate) the log file with its header
	// https://en.cppreference.com/w/cpp/io/basic_ofstream
	std::ofstream out(this->name + ".txt", std::ios::trunc);
	out << "Process name: " << this->name << std::endl
		<< "Logs:" << std::endl << std::endl;
}

void Process::print(int core) {
	// open and close per line so every line is written right away
	std::ofstream out(this->name + ".txt", std::ios::app);
	out << "(" << timeNow() << ") Core:" << core
		<< " \"Hello world from " << this->name << "!\"" << std::endl;
	this->executed++;
}

std::string Process::getName() const {
	return this->name;
}

std::string Process::getCreated() const {
	return this->created;
}

int Process::getTotal() const {
	return this->total;
}

int Process::getExecuted() const {
	return this->executed.load();
}

// Scheduler implementation
Scheduler::Scheduler(int numCores) {
	this->cores.resize(numCores);
	this->running = false;
}

Scheduler::~Scheduler() {
	this->stop();
}

void Scheduler::addProcess(const std::string& name, int instructions) {
	// build the process before taking the lock (creates its log file)
	std::shared_ptr<Process> proc = std::make_shared<Process>(name, instructions);
	std::lock_guard<std::mutex> lk(this->mtx);
	this->readyQueue.push(proc);
}

void Scheduler::start() {
	// if running already, do nothing
	if (this->running.load()) return;
	this->running.store(true);

	// start the scheduler thread and one worker thread per core
	// https://en.cppreference.com/w/cpp/thread/thread/thread
	this->schedulerThread = std::thread(&Scheduler::schedule, this);
	for (std::size_t i = 0; i < this->cores.size(); ++i) {
		this->coreThreads.emplace_back(&Scheduler::runCore, this, static_cast<int>(i));
	}
}

void Scheduler::stop() {
	if (!this->running.load()) return;
	this->running.store(false);

	// wait for every thread to finish
	// https://en.cppreference.com/w/cpp/thread/thread/join
	if (this->schedulerThread.joinable()) {
		this->schedulerThread.join();
	}
	for (std::thread& t : this->coreThreads) {
		if (t.joinable()) {
			t.join();
		}
	}
	this->coreThreads.clear();
}

void Scheduler::schedule() {
	while (this->running.load()) {
		{
			std::lock_guard<std::mutex> lk(this->mtx);
			// FCFS: give each idle core (lowest id first) the next process in the queue
			for (std::size_t i = 0; i < this->cores.size() && !this->readyQueue.empty(); ++i) {
				if (!this->cores[i]) {
					this->cores[i] = this->readyQueue.front();
					this->readyQueue.pop();
				}
			}
		}
		// poll every 10 ms
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void Scheduler::runCore(int core) {
	while (this->running.load()) {
		// get the process assigned to this core
		std::shared_ptr<Process> proc;
		{
			std::lock_guard<std::mutex> lk(this->mtx);
			proc = this->cores[core];
		}

		// core is idle, wait for the scheduler to assign a process
		if (!proc) {
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			continue;
		}

		// non-preemptive: run every instruction before the core is released
		while (proc->getExecuted() < proc->getTotal()) {
			if (!this->running.load()) return; // stopped mid-run
			// each instruction takes 50 ms
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			proc->print(core);
		}

		// release the core and record the process as finished
		std::lock_guard<std::mutex> lk(this->mtx);
		this->cores[core] = nullptr;
		this->finished.push_back(proc);
	}
}

void Scheduler::screenList() {
	std::ostringstream out;
	out << std::left;
	std::string line(38, '-');
	out << line << std::endl << "Running processes:" << std::endl;
	{
		std::lock_guard<std::mutex> lk(this->mtx);
		// running processes, ordered by core id
		for (std::size_t i = 0; i < this->cores.size(); ++i) {
			const std::shared_ptr<Process>& proc = this->cores[i];
			if (!proc) continue;
			out << std::setw(12) << proc->getName()
				<< "(" << proc->getCreated() << ")   "
				<< std::setw(11) << ("Core: " + std::to_string(i))
				<< proc->getExecuted() << " / " << proc->getTotal() << std::endl;
		}

		out << std::endl << "Finished processes:" << std::endl;
		// finished processes, in completion order
		for (const std::shared_ptr<Process>& proc : this->finished) {
			out << std::setw(12) << proc->getName()
				<< "(" << proc->getCreated() << ")   "
				<< std::setw(11) << "Finished"
				<< proc->getExecuted() << " / " << proc->getTotal() << std::endl;
		}
	}
	out << line << std::endl;

	// print while holding the console lock
	std::lock_guard<std::mutex> lk(Marquee::ConsoleMutex());
	std::cout << out.str() << std::flush;
}
