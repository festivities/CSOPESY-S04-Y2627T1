#include "OperatingSystem.h"
#include <windows.h>

// Constructor implementation
OperatingSystem::OperatingSystem() {
	// set the supported commands and descriptions
	// not necessary, wanted to do something like if cmd is in commands -> do something
	this->commands = {
		{"exit", "Exit the program"},
		{"clear", "Clear the screen"},
		{"initialize", "Initialize the operating system"},
		{"screen", "Something"},
		{"scheduler-start", "Start the scheduler"},
		{"scheduler-stop", "Stop the scheduler"},
		{"report-util", "Something"}
	};
}

// Getter implementation
std::map<std::string, std::string> OperatingSystem::getCommands() const {
	return this->commands;
}

// Command implementations
void OperatingSystem::initialize() const {
	std::cout << "\"initialize\" command recognized. Doing something." << std::endl;
}

void OperatingSystem::screen() const {
	std::cout << "\"screen\" command recognized. Doing something." << std::endl;
}

void OperatingSystem::schedulerStart() const {
	std::cout << "\"scheduler-start\" command recognized. Doing something." << std::endl;
}

void OperatingSystem::schedulerStop() const {
	std::cout << "\"scheduler-stop\" command recognized. Doing something." << std::endl;
}

void OperatingSystem::reportUtil() const {
	std::cout << "\"report-util\" command recognized. Doing something." << std::endl;
}

// Display menu header 
void OperatingSystem::printMainMenu() const {
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	std::cout << R"(   ___________ ____  ____  _____________  __
  / ____/ ___// __ \/ __ \/ ____/ ___/\ \/ /
 / /    \__ \/ / / / /_/ / __/  \__ \  \  /
/ /___ ___/ / /_/ / ____/ /___ ___/ /  / /
\____//____/\____/_/   /_____//____/  /_/
)" << std::endl;

	// green 
	SetConsoleTextAttribute(h, FOREGROUND_GREEN);
	std::cout << "Hello, Welcome to CSOPESY - GROUP 6 Command Line!" << std::endl;

	// yellow
	SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	std::cout << "Type 'exit' to quit, 'clear' to clear the screen." << std::endl << std::endl;
	std::cout << "** IMPORTANT: Type 'initialize' to load config and start system **" << std::endl;

	// default white
	SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}