// CSOPESY S04-Y2627T1 - GROUP 6
// This file contains the implementation of the OperatingSystem class, which manages
// the CLI commands, config.txt, the process screens and the marquee animation.
//
#include "OperatingSystem.h"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <stdexcept>

// largest value allowed by the config keys that have a 2^32 limit
static const long long maxConfigValue = 4294967296LL;

// Helper to remove leading and trailing whitespace from string
std::string trim(const std::string& text)
{
	const char* WhiteSpace = " \t\v\r\n";
	std::size_t start = text.find_first_not_of(WhiteSpace);
	std::size_t end = text.find_last_not_of(WhiteSpace);
	return start == std::string::npos ? std::string() : text.substr(start, end - start + 1);
}

// Helper to read a number from config text. Returns false if the text is not a number
// https://en.cppreference.com/w/cpp/string/basic_string/stol
static bool parseNumber(const std::string& text, long long& number) {
	try {
		number = std::stoll(text);
		return true;
	}
	catch (const std::logic_error&) {
		return false;
	}
}

// keep a config number inside [low, high]
static uint64_t clampToRange(long long number, long long low, long long high) {
	if (number < low) number = low;
	if (number > high) number = high;
	return static_cast<uint64_t>(number);
}

// Constructor implementation
OperatingSystem::OperatingSystem() : marquee(30) {
	this->initialized = false;

	// set the supported commands and descriptions
	this->commands = {
		{"help", "Display available commands"},
		{"clear", "Clear the screen"},
		{"exit", "Exit the program"},

		{"initialize", "Initialize the operating system"},
		{"screen -s <name>", "Create process <name> and open its screen"},
		{"screen -r <name>", "Open the screen of running process <name>"},
		{"screen -ls", "List running and finished processes"},
		{"scheduler-start", "Start the scheduler"},
		{"scheduler-stop", "Stop the scheduler"},
		{"report-util", "Report system utilization"},

		// MARQUEE CONTROL COMMANDS
		{"start_marquee", "Start marquee text animation"},
		{"stop_marquee", "Stop marquee text animation"},
		{"set_text <text>", "Set <text> as the text to be displayed in the marquee animation"},
		{"set_speed <speed>", "Set <speed> as the speed of the marquee animation in milliseconds"},
	};

	// marquee is 30 characters wide by default, adjust if need
}

// Getter implementation
std::map<std::string, std::string> OperatingSystem::getCommands() const {
	return this->commands;
}

bool OperatingSystem::isInitialized() const {
	return this->initialized;
}

// Command implementations
void OperatingSystem::initialize() {
	// only one initialization per run
	if (this->initialized) {
		std::cout << "Emulator is already initialized." << std::endl;
		return;
	}

	// config.txt is read from the working directory
	std::ifstream in("config.txt");
	if (!in) {
		std::cout << "Error: config.txt not found." << std::endl;
		return;
	}

	// start from the defaults (same values as the sample config.txt)
	SchedulerConfig cfg;

	// read key value pairs. Unknown keys and values that are not numbers are ignored
	std::string key;
	std::string value;
	while (in >> key >> value) {
		if (key == "scheduler") {
			// the value may be quoted, e.g. "rr"
			cfg.scheduler.clear();
			for (char ch : value) {
				if (ch != '"') cfg.scheduler += ch;
			}
			continue;
		}

		long long number = 0;
		if (!parseNumber(value, number)) continue;

		if (key == "num-cpu") {
			cfg.numCpu = clampToRange(number, 1, 128);
		}
		else if (key == "quantum-cycles") {
			cfg.quantumCycles = clampToRange(number, 1, maxConfigValue);
		}
		else if (key == "batch-process-freq") {
			cfg.batchProcessFreq = clampToRange(number, 1, maxConfigValue);
		}
		else if (key == "min-ins") {
			cfg.minIns = clampToRange(number, 1, maxConfigValue);
		}
		else if (key == "max-ins") {
			cfg.maxIns = clampToRange(number, 1, maxConfigValue);
		}
		else if (key == "delays-per-exec" || key == "delay-per-exec") {
			cfg.delayPerExec = clampToRange(number, 0, maxConfigValue);
		}
		else if (key == "tick-ms") {
			cfg.tickMs = clampToRange(number, 1, 1000);
		}
	}

	// max-ins can not be smaller than min-ins
	if (cfg.maxIns < cfg.minIns) {
		cfg.maxIns = cfg.minIns;
	}

	if (cfg.scheduler != "fcfs" && cfg.scheduler != "rr") {
		std::cout << "Error: scheduler must be \"fcfs\" or \"rr\"." << std::endl;
		return;
	}

	// show the loaded values
	std::cout << "num-cpu: " << cfg.numCpu << std::endl
		<< "scheduler: " << cfg.scheduler << std::endl
		<< "quantum-cycles: " << cfg.quantumCycles << std::endl
		<< "batch-process-freq: " << cfg.batchProcessFreq << std::endl
		<< "min-ins: " << cfg.minIns << std::endl
		<< "max-ins: " << cfg.maxIns << std::endl
		<< "delay-per-exec: " << cfg.delayPerExec << std::endl
		<< "tick-ms: " << cfg.tickMs << std::endl;

	// start the scheduler. It keeps running until the program exits
	this->scheduler.start(cfg);
	this->initialized = true;
}

// screen -s <name>: create a new process and open its screen
void OperatingSystem::screenCreate(const std::string& name) {
	std::shared_ptr<Process> proc = this->scheduler.addNamedProcess(name);
	if (!proc) {
		std::cout << "Process " << name << " already exists." << std::endl;
		return;
	}
	this->processScreen(proc);
}

// screen -r <name>: open the screen of a process that has not finished
void OperatingSystem::screenAttach(const std::string& name) {
	std::shared_ptr<Process> proc = this->scheduler.findAttachable(name);
	if (!proc) {
		std::cout << "Process " << name << " not found." << std::endl;
		return;
	}
	this->processScreen(proc);
}

// process screen: process-smi shows the process, exit goes back to the main menu
void OperatingSystem::processScreen(const std::shared_ptr<Process>& proc) {
	std::system("cls");

	// show the process once when the screen opens
	std::string text = this->scheduler.processSmi(proc);
	{
		std::lock_guard<std::mutex> lk(Marquee::ConsoleMutex());
		std::cout << text << std::endl;
	}

	std::string line;
	while (true) {
		// prompt (lock console while doing so)
		{
			std::lock_guard<std::mutex> lk(Marquee::ConsoleMutex());
			std::cout << "root:\\> " << std::flush;
		}
		// end of input leaves the screen
		if (!std::getline(std::cin, line)) return;

		line = trim(line);
		if (line == "process-smi") {
			text = this->scheduler.processSmi(proc);
			std::lock_guard<std::mutex> lk(Marquee::ConsoleMutex());
			std::cout << text << std::endl;
		}
		else if (line == "exit") {
			// back to the main menu
			this->clearScreen();
			return;
		}
		else {
			std::cout << "Command \"" << line << "\" not recognized." << std::endl;
		}
	}
}

void OperatingSystem::schedulerStart() {
	if (this->scheduler.startGenerating()) {
		std::cout << "Scheduler started generating processes." << std::endl;
	}
	else {
		std::cout << "Scheduler is already running." << std::endl;
	}
}

void OperatingSystem::schedulerStop() {
	if (this->scheduler.stopGenerating()) {
		std::cout << "Scheduler stopped generating processes." << std::endl;
	}
	else {
		std::cout << "Scheduler is not running." << std::endl;
	}
}

// report-util: save the report to csopesy-log.txt (overwritten every time)
void OperatingSystem::reportUtil() {
	std::string text = this->scheduler.report();

	std::ofstream out("csopesy-log.txt", std::ios::trunc);
	if (!out) {
		std::cout << "Error: could not write csopesy-log.txt." << std::endl;
		return;
	}
	out << text;
	out.close();

	std::lock_guard<std::mutex> lk(Marquee::ConsoleMutex());
	std::cout << "Report generated at " << std::filesystem::absolute("csopesy-log.txt").string() << "!" << std::endl;
}

void OperatingSystem::clearScreen() const {
	if (this->marquee.getRunning()) {
		// clear but keep marquee running
		std::lock_guard<std::mutex> lk(Marquee::ConsoleMutex());
		std::cout << "\033[2J\033[2;1H" << std::flush;

	}
	else {
		// print main menu if marquee not running
		std::system("cls");
		OperatingSystem::printMainMenu();
	}
}

// Display menu header
void OperatingSystem::printMainMenu() const {
	std::cout << R"(   ___________ ____  ____  _____________  __
  / ____/ ___// __ \/ __ \/ ____/ ___/\ \/ /
 / /    \__ \/ / / / /_/ / __/  \__ \  \  /
/ /___ ___/ / /_/ / ____/ /___ ___/ /  / /
\____//____/\____/_/   /_____//____/  /_/
)" << std::endl;

	std::cout << "Welcome to CSOPESY OS Emulator!" << std::endl << std::endl;
	std::cout << "Group developer:" << std::endl;
	std::cout << "Ong, David Zachary" << std::endl;
	std::cout << "Ho, Denise Liana" << std::endl;
	std::cout << "Valle, Jose Edgardo" << std::endl;
	std::cout << "Biacora, Luis Gabriel" << std::endl << std::endl;
	std::cout << "Version date:" << std::endl;
	std::cout << "10/31/2026" << std::endl;
}

// Display commands
void OperatingSystem::printCommands() const {
	std::cout << "Available commands:" << std::endl;
	for (const auto& [command, description] : this->commands) {
		std::cout << "  "
			<< std::left << std::setw(20) << command // left align cmd names and set width = 20
			<< ": " << description << std::endl;
	}
}

// Marquee control
void OperatingSystem::startMarquee() {
	this->marquee.start();
}

void OperatingSystem::stopMarquee() {
	this->marquee.stop();
	std::cout << "Marquee animation stopped." << std::endl;

	this->clearScreen();
}

void OperatingSystem::setMarqueeText(const std::string& text) {
	this->marquee.setText(text);
	std::cout << "Marquee text set to: \"" << text << "\"" << std::endl;
}

void OperatingSystem::setMarqueeSpeed(int speed) {
	this->marquee.setSpeed(speed);
	std::cout << "Marquee speed set to: " << speed << " ms" << std::endl;
}

// screen -ls: print the report
void OperatingSystem::screenList() {
	std::string text = this->scheduler.report();
	std::lock_guard<std::mutex> lk(Marquee::ConsoleMutex());
	std::cout << text << std::flush;
}
