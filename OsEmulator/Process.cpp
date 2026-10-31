// CSOPESY S04-Y2627T1 - GROUP 6
// This file contains the implementation of the Process class: random instruction
// generation, running one CPU tick and the process-smi text.
//

#include "Process.h"
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <random>
#include <sstream>

// variable name pool (Operand.var is an index into this list)
static const std::string varNames[] = {"x", "y", "z", "a", "b", "c"};
static const uint64_t varCount = sizeof(varNames) / sizeof(varNames[0]);

// one random number generator for all processes. Every call happens under the scheduler mutex
// https://en.cppreference.com/w/cpp/numeric/random/mersenne_twister_engine
static std::mt19937_64 rng(std::random_device{}());

// random integer in [low, high]
// https://en.cppreference.com/w/cpp/numeric/random/uniform_int_distribution
static uint64_t randomBetween(uint64_t low, uint64_t high) {
	std::uniform_int_distribution<uint64_t> dist(low, high);
	return dist(rng);
}

// Helper to get the current local time as %m/%d/%Y %I:%M:%S%p
// https://en.cppreference.com/w/cpp/io/manip/put_time
static std::string timeNow() {
	std::time_t now = std::time(nullptr);
	std::tm local{};
	// https://en.cppreference.com/w/c/chrono/localtime
	localtime_s(&local, &now);
	std::ostringstream out;
	out << std::put_time(&local, "%m/%d/%Y %I:%M:%S%p");
	return out.str();
}

// clamp a result into the uint16 range [0, 65535]
static uint16_t clampToUint16(int64_t value) {
	if (value < 0) return 0;
	if (value > 65535) return 65535;
	return static_cast<uint16_t>(value);
}

// random variable operand from the name pool
static Operand randomVarOperand() {
	Operand op{};
	op.isVar = true;
	op.var = static_cast<uint8_t>(randomBetween(0, varCount - 1));
	return op;
}

// random operand: a variable half of the time, otherwise a literal in [0, 100]
static Operand randomValueOperand() {
	if (randomBetween(0, 1) == 1) {
		return randomVarOperand();
	}
	Operand op{};
	op.value = static_cast<uint16_t>(randomBetween(0, 100));
	return op;
}

// make one random simple instruction (PRINT, DECLARE, ADD, SUBTRACT or SLEEP)
static Instruction randomSimple() {
	Instruction ins{};
	// PRINT, DECLARE, ADD, SUBTRACT and SLEEP are the first five values of the enum
	ins.type = static_cast<InstructionType>(randomBetween(0, 4));
	switch (ins.type) {
	case DECLARE:
		ins.a = randomVarOperand();
		ins.b.value = static_cast<uint16_t>(randomBetween(0, 65535));
		break;
	case ADD:
	case SUBTRACT:
		ins.a = randomVarOperand();
		ins.b = randomValueOperand();
		ins.c = randomValueOperand();
		break;
	case SLEEP:
		ins.a.value = static_cast<uint16_t>(randomBetween(1, 10));
		break;
	default:
		// PRINT has no operands
		break;
	}
	return ins;
}

// Process implementation
Process::Process(const std::string& name, int id, uint64_t minIns, uint64_t maxIns) {
	this->id = id;
	this->name = name;
	this->created = timeNow();
	this->core = -1;

	// pick how many lines this process will run. The instructions are made on the first run
	this->totalLines = randomBetween(minIns, maxIns);
	this->executedLines = 0;
	this->delayLeft = 0;
	this->quantumUsed = 0;
	this->wakeTick = 0;
	this->generated = false;
	this->finished = false;
	this->pc = 0;
}

// add instructions for `remaining` executed lines. FOR loops are nested at most 3 deep
void Process::generate(uint64_t remaining, int depth) {
	while (remaining > 0) {
		// 1 in 10 chance of a loop (only with room for a body and while nesting is below 3)
		if (depth < 3 && remaining >= 4 && randomBetween(1, 10) == 1) {
			uint64_t repeats = randomBetween(2, 4);
			uint64_t bodyLines = randomBetween(1, std::min<uint64_t>(remaining / repeats, 8));

			// emit the FOR header now and fill in the body length after the body is made
			std::size_t header = this->instructions.size();
			Instruction loop{};
			loop.type = FOR;
			loop.a.value = static_cast<uint16_t>(repeats);
			this->instructions.push_back(loop);

			this->generate(bodyLines, depth + 1);
			this->instructions[header].b.value = static_cast<uint16_t>(this->instructions.size() - header - 1);

			remaining -= repeats * bodyLines;
		}
		else {
			// note: every instruction is stored, so a huge max-ins uses a lot of memory
			this->instructions.push_back(randomSimple());
			remaining--;
		}
	}
}

uint16_t Process::readOperand(const Operand& op) {
	if (!op.isVar) {
		return op.value;
	}
	// undeclared variables read as 0 (operator[] adds them with value 0)
	return this->variables[varNames[op.var]];
}

// run one CPU tick on the given core
TickResult Process::runTick(int cpu, uint64_t delay, uint64_t tick) {
	this->quantumUsed++;

	// first run on a core: build the instruction list
	if (!this->generated) {
		this->generate(this->totalLines, 0);
		this->generated = true;
	}

	// busy-wait (delay-per-exec): stay on the core and do nothing this tick
	if (this->delayLeft > 0) {
		this->delayLeft--;
		return RUNNING;
	}

	// FOR headers take no tick: push their loop frames and go on to the next instruction
	while (this->pc < this->instructions.size() && this->instructions[this->pc].type == FOR) {
		const Instruction& header = this->instructions[this->pc];
		LoopFrame frame;
		frame.startPc = this->pc + 1;
		frame.endPc = this->pc + header.b.value;
		frame.remaining = header.a.value;
		this->loops.push_back(frame);
		this->pc++;
	}

	// run the instruction at pc. This counts as one executed line
	const Instruction ins = this->instructions[this->pc];
	this->pc++;
	this->executedLines++;
	this->delayLeft = delay;

	bool sleeping = false;
	if (ins.type == PRINT) {
		// PRINT(a): a plain hello, or the value of one variable
		std::string msg;
		if (ins.a.isVar) {
			msg = "Value from " + this->name + ": " + varNames[ins.a.var] + " = " + std::to_string(this->readOperand(ins.a));
		}
		else {
			msg = "Hello world from " + this->name + "!";
		}
		this->logs.push_back("(" + timeNow() + ") Core:" + std::to_string(cpu) + " \"" + msg + "\"");
	}
	else if (ins.type == DECLARE) {
		// DECLARE(a = var, b = value)
		this->variables[varNames[ins.a.var]] = this->readOperand(ins.b);
	}
	else if (ins.type == ADD) {
		// ADD(a = var, b, c): add in 64 bits, then clamp to uint16
		int64_t sum = static_cast<int64_t>(this->readOperand(ins.b)) + this->readOperand(ins.c);
		this->variables[varNames[ins.a.var]] = clampToUint16(sum);
	}
	else if (ins.type == SUBTRACT) {
		// SUBTRACT(a = var, b, c): subtract in 64 bits, then clamp to uint16
		int64_t diff = static_cast<int64_t>(this->readOperand(ins.b)) - this->readOperand(ins.c);
		this->variables[varNames[ins.a.var]] = clampToUint16(diff);
	}
	else if (ins.type == SLEEP) {
		// give up the core for a.value ticks
		sleeping = true;
		this->wakeTick = tick + ins.a.value;
	}

	// reached the end of a loop body: repeat it, or leave the loop when it is done.
	// nested loops can end on the same instruction, so keep checking the outer loops
	while (!this->loops.empty() && this->pc == this->loops.back().endPc + 1) {
		this->loops.back().remaining--;
		if (this->loops.back().remaining > 0) {
			this->pc = this->loops.back().startPc;
			break;
		}
		this->loops.pop_back();
	}

	// ran past the last instruction: the process is done
	if (this->pc >= this->instructions.size()) {
		this->finished = true;
		// free what is not needed any more (name, logs and counters stay for process-smi)
		this->instructions.clear();
		this->instructions.shrink_to_fit();
		this->loops.clear();
		this->loops.shrink_to_fit();
		this->variables.clear();
		return FINISHED;
	}

	if (sleeping) {
		return SLEEPING;
	}
	return RUNNING;
}

// the scheduler put this process on a core: start a new time slice
void Process::enterCore(int cpu) {
	this->core = cpu;
	this->quantumUsed = 0;
}

void Process::leaveCore() {
	this->core = -1;
}

std::string Process::smi() const {
	std::ostringstream out;
	out << "Process name: " << this->name << std::endl
		<< "ID: " << this->id << std::endl
		<< "Logs:" << std::endl;
	for (const std::string& line : this->logs) {
		out << line << std::endl;
	}
	out << std::endl;

	if (this->finished) {
		out << "Finished!" << std::endl;
	}
	else {
		out << "Current instruction line: " << this->executedLines << std::endl
			<< "Lines of code: " << this->totalLines << std::endl;
	}
	return out.str();
}

// Getter implementations
int Process::getId() const {
	return this->id;
}

std::string Process::getName() const {
	return this->name;
}

std::string Process::getCreated() const {
	return this->created;
}

int Process::getCore() const {
	return this->core;
}

uint64_t Process::getExecutedLines() const {
	return this->executedLines;
}

uint64_t Process::getTotalLines() const {
	return this->totalLines;
}

uint64_t Process::getQuantumUsed() const {
	return this->quantumUsed;
}

uint64_t Process::getWakeTick() const {
	return this->wakeTick;
}

bool Process::isFinished() const {
	return this->finished;
}
