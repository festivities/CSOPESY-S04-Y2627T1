// CSOPESY S04-Y2627T1 - GROUP 6
// This header file contains the declarations of the Process class and the instruction
// types a process runs (PRINT, DECLARE, ADD, SUBTRACT, SLEEP and FOR).
//

#pragma once

#include <cstddef>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

// instruction types. PRINT to SLEEP are generated randomly, FOR is a loop header
enum InstructionType { PRINT, DECLARE, ADD, SUBTRACT, SLEEP, FOR };

// a value used by an instruction: a variable (index into the name pool) or a literal
struct Operand {
	bool isVar;
	uint8_t var;
	uint16_t value;
};

// one flat instruction. For FOR: a.value = repeats, b.value = number of instructions in the body
struct Instruction {
	InstructionType type;
	Operand a;
	Operand b;
	Operand c;
};

// one active loop. The body is the instructions from startPc to endPc (both included)
// and remaining is the number of iterations still to run
struct LoopFrame {
	std::size_t startPc;
	std::size_t endPc;
	int remaining;
};

// what happened to a process during one CPU tick
enum TickResult { RUNNING, SLEEPING, FINISHED };

// a process with generated instructions. It runs on a core one tick at a time
class Process {
private:
	// process id (1, 2, 3...) and name (p01, screen_01...)
	int id;
	std::string name;
	// creation time formatted as %m/%d/%Y %I:%M:%S%p
	std::string created;
	// core the process is on (-1 when it is not on a core)
	int core;

	// executed lines so far and the total number of lines the process will run
	uint64_t executedLines;
	uint64_t totalLines;
	// ticks left of the busy-wait before the next instruction (delay-per-exec)
	uint64_t delayLeft;
	// ticks used on the current core (round robin time slice)
	uint64_t quantumUsed;
	// tick at which a sleeping process wakes up
	uint64_t wakeTick;
	// true once the instruction list has been generated (first run on a core)
	bool generated;
	bool finished;

	// flat instruction list, program counter and loop stack
	std::vector<Instruction> instructions;
	std::size_t pc;
	std::vector<LoopFrame> loops;
	// variables by name, freed when the process finishes
	// https://en.cppreference.com/w/cpp/container/map
	std::map<std::string, uint16_t> variables;

	// log lines written by PRINT, e.g. (10/31/2026 09:15:22AM) Core:0 "Hello world from p01!"
	std::vector<std::string> logs;

	// add instructions for `remaining` executed lines (recursive for FOR loops)
	void generate(uint64_t remaining, int depth);
	// value of an operand (undeclared variables read as 0 and get declared)
	uint16_t readOperand(const Operand& op);

public:
	// constructor (picks the number of lines, the instructions are made on the first run)
	Process(const std::string& name, int id, uint64_t minIns, uint64_t maxIns);

	// run one CPU tick on the given core and report what happened
	TickResult runTick(int cpu, uint64_t delay, uint64_t tick);

	// the scheduler puts this process on a core / takes it off
	void enterCore(int cpu);
	void leaveCore();

	// text for process-smi: name, id, logs and progress (or Finished!)
	std::string smi() const;

	// getters
	int getId() const;
	std::string getName() const;
	std::string getCreated() const;
	int getCore() const;
	uint64_t getExecutedLines() const;
	uint64_t getTotalLines() const;
	uint64_t getQuantumUsed() const;
	uint64_t getWakeTick() const;
	bool isFinished() const;
};
