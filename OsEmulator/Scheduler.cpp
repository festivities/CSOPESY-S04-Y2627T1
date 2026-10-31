// CSOPESY S04-Y2627T1 - GROUP 6
// This file contains the implementation of the Scheduler class. One scheduler thread runs
// a tick loop that moves processes between the ready queue, the cores and the sleep list.
// The scheduler thread never prints to the console.
//

#include "Scheduler.h"
#include <chrono>
#include <iomanip>
#include <sstream>

// Scheduler implementation
Scheduler::Scheduler() {
	this->tick = 0;
	this->genCounter = 0;
	this->generating = false;
	this->nextNameNumber = 1;
	this->running = false;
}

Scheduler::~Scheduler() {
	this->stop();
}

void Scheduler::start(const SchedulerConfig& cfg) {
	// if running already, do nothing
	if (this->running.load()) return;

	// one core slot per cpu, all idle
	this->config = cfg;
	this->cores.resize(static_cast<std::size_t>(this->config.numCpu));

	// start the tick loop. It keeps running until the program exits
	this->running.store(true);
	this->schedulerThread = std::thread(&Scheduler::run, this);
}

void Scheduler::stop() {
	if (!this->running.load()) return;
	this->running.store(false);

	// wait for the tick loop to end (it checks the flag every tick)
	// https://en.cppreference.com/w/cpp/thread/thread/join
	if (this->schedulerThread.joinable()) {
		this->schedulerThread.join();
	}
}

bool Scheduler::startGenerating() {
	std::lock_guard<std::mutex> lk(this->mtx);
	if (this->generating) return false;
	this->generating = true;
	return true;
}

bool Scheduler::stopGenerating() {
	std::lock_guard<std::mutex> lk(this->mtx);
	if (!this->generating) return false;
	this->generating = false;
	return true;
}

std::shared_ptr<Process> Scheduler::addNamedProcess(const std::string& name) {
	std::lock_guard<std::mutex> lk(this->mtx);
	// a name can be reused only after its process finished
	std::shared_ptr<Process> existing = this->findProcess(name);
	if (existing && !existing->isFinished()) return nullptr;
	return this->makeProcess(name);
}

std::shared_ptr<Process> Scheduler::findAttachable(const std::string& name) {
	std::lock_guard<std::mutex> lk(this->mtx);
	std::shared_ptr<Process> found = this->findProcess(name);
	if (!found || found->isFinished()) return nullptr;
	return found;
}

std::string Scheduler::processSmi(const std::shared_ptr<Process>& proc) {
	std::lock_guard<std::mutex> lk(this->mtx);
	return proc->smi();
}

// Tick loop: one pass of the loop is one CPU tick
void Scheduler::run() {
	while (this->running.load()) {
		{
			std::lock_guard<std::mutex> lk(this->mtx);

			// 1. sleeping processes whose wake-up tick has come go to the back of the ready queue
			std::vector<std::shared_ptr<Process>> stillSleeping;
			for (const std::shared_ptr<Process>& proc : this->sleeping) {
				if (proc->getWakeTick() <= this->tick) {
					this->readyQueue.push(proc);
				}
				else {
					stillSleeping.push_back(proc);
				}
			}
			this->sleeping = stillSleeping;

			// 2. while generating, make a new process every batch-process-freq ticks
			if (this->generating) {
				this->genCounter++;
				if (this->genCounter % this->config.batchProcessFreq == 0) {
					this->makeProcess(this->nextAutoName());
				}
			}

			// 3. every busy core runs one tick of its process
			for (std::size_t i = 0; i < this->cores.size(); i++) {
				std::shared_ptr<Process> proc = this->cores[i];
				if (!proc) continue;

				TickResult result = proc->runTick(static_cast<int>(i), this->config.delayPerExec, this->tick);
				// round robin: the process gives up the core when its time slice is used up
				bool timeUp = this->config.scheduler == "rr" && proc->getQuantumUsed() >= this->config.quantumCycles;

				if (result == FINISHED) {
					this->finished.push_back(proc);
				}
				else if (result == SLEEPING) {
					this->sleeping.push_back(proc);
				}
				else if (timeUp) {
					this->readyQueue.push(proc);
				}
				else {
					// still running, keeps the core
					continue;
				}

				// the process left the core
				proc->leaveCore();
				this->cores[i] = nullptr;
			}

			// 4. every idle core (lowest id first) takes the next process in the ready queue
			for (std::size_t i = 0; i < this->cores.size() && !this->readyQueue.empty(); i++) {
				if (this->cores[i]) continue;
				std::shared_ptr<Process> proc = this->readyQueue.front();
				this->readyQueue.pop();
				proc->enterCore(static_cast<int>(i));
				this->cores[i] = proc;
			}

			// 5. next tick
			this->tick++;
		}

		// one CPU tick lasts tick-ms milliseconds (10 by default)
		std::this_thread::sleep_for(std::chrono::milliseconds(this->config.tickMs));
	}
}

// create a process with the next id and put it in the ready queue
std::shared_ptr<Process> Scheduler::makeProcess(const std::string& name) {
	int id = static_cast<int>(this->all.size()) + 1;
	std::shared_ptr<Process> proc = std::make_shared<Process>(name, id, this->config.minIns, this->config.maxIns);
	this->all.push_back(proc);
	this->readyQueue.push(proc);
	return proc;
}

// newest process with this name (nullptr if there is none)
std::shared_ptr<Process> Scheduler::findProcess(const std::string& name) {
	std::shared_ptr<Process> found;
	for (const std::shared_ptr<Process>& proc : this->all) {
		if (proc->getName() == name) {
			found = proc;
		}
	}
	return found;
}

// next generated name (p01, p02, ... p100, ...), skipping names that are already in use
std::string Scheduler::nextAutoName() {
	while (true) {
		std::string number = std::to_string(this->nextNameNumber);
		this->nextNameNumber++;
		if (number.size() < 2) {
			number = "0" + number;
		}
		std::string name = "p" + number;
		if (!this->findProcess(name)) {
			return name;
		}
	}
}

// text for screen -ls and report-util
std::string Scheduler::report() {
	std::ostringstream out;
	std::string line(38, '-');
	std::lock_guard<std::mutex> lk(this->mtx);

	// cores in use and the utilization
	std::size_t used = 0;
	for (const std::shared_ptr<Process>& proc : this->cores) {
		if (proc) used++;
	}
	std::size_t total = this->cores.size();
	out << "CPU utilization: " << (used * 100 / total) << "%" << std::endl
		<< "Cores used: " << used << std::endl
		<< "Cores available: " << (total - used) << std::endl
		<< std::endl << line << std::endl;

	// running processes, ordered by core id
	out << std::left << "Running processes:" << std::endl;
	for (const std::shared_ptr<Process>& proc : this->cores) {
		if (!proc) continue;
		out << std::setw(12) << proc->getName()
			<< "(" << proc->getCreated() << ")   "
			<< std::setw(11) << ("Core: " + std::to_string(proc->getCore()))
			<< proc->getExecutedLines() << " / " << proc->getTotalLines() << std::endl;
	}

	// finished processes, in completion order
	out << std::endl << "Finished processes:" << std::endl;
	for (const std::shared_ptr<Process>& proc : this->finished) {
		out << std::setw(12) << proc->getName()
			<< "(" << proc->getCreated() << ")   "
			<< std::setw(11) << "Finished"
			<< proc->getExecutedLines() << " / " << proc->getTotalLines() << std::endl;
	}
	out << line << std::endl;

	return out.str();
}
