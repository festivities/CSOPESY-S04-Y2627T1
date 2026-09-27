#include "OperatingSystem.h"
#include <iomanip> 

// Constructor implementation
OperatingSystem::OperatingSystem() : marquee(30, 10) {
	// set the supported commands and descriptions
	this->commands = {
		{"help", "Display available commands"},
		{"clear", "Clear the screen"},
		{"exit", "Exit the program"},

		{"initialize", "Initialize the operating system"},
		{"screen", "Something"},
		{"scheduler-start", "Start the scheduler"},
		{"scheduler-stop", "Stop the scheduler"},
		{"report-util", "Something"},

		// MARQUEE CONTROL COMMANDS
		{"start_marquee", "Start marquee text animation"},
		{"stop_marquee", "Stop marquee text animation"},
		{"set_text <text>", "Set <text> as the text to be displayed in the marquee animation"},
		{"set_speed <speed>", "Set <speed> as the speed of the marquee animation in milliseconds"},
	};

	// marquee is 30x10 pixels, adjust if need
}

// Getter implementation
std::map<std::string, std::string> OperatingSystem::getCommands() const {
	return this->commands;
}

// Command implementations
void OperatingSystem::initialize() const {
	std::cout << "initialize command recognized. Doing something." << std::endl;
}

void OperatingSystem::screen() const {
	std::cout << "screen command recognized. Doing something." << std::endl;
}

void OperatingSystem::schedulerStart() const {
	std::cout << "scheduler-start command recognized. Doing something." << std::endl;
}

void OperatingSystem::schedulerStop() const {
	std::cout << "scheduler-stop command recognized. Doing something." << std::endl;
}

void OperatingSystem::reportUtil() const {
	std::cout << "report-util command recognized. Doing something." << std::endl;
}

// Display menu header
void OperatingSystem::printMainMenu() const {
	std::cout << R"(   ___________ ____  ____  _____________  __
  / ____/ ___// __ \/ __ \/ ____/ ___/\ \/ /
 / /    \__ \/ / / / /_/ / __/  \__ \  \  /
/ /___ ___/ / /_/ / ____/ /___ ___/ /  / /
\____//____/\____/_/   /_____//____/  /_/
)" << std::endl;

	std::cout << "Hello, Welcome to CSOPESY - GROUP 6 Command Line!" << std::endl;
	std::cout << "Type 'exit' to quit, 'clear' to clear the screen." << std::endl;
	//std::cout << "** IMPORTANT: Type 'initialize' to load config and start system **" << std::endl;
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
}

void OperatingSystem::setMarqueeText(const std::string& text) {
	this->marquee.setText(text);
	std::cout << "Marquee text set to: \"" << text << "\"" << std::endl;
}

void OperatingSystem::setMarqueeSpeed(int speed) {
	this->marquee.setSpeed(speed);
	std::cout << "Marquee speed set to: " << speed << " ms" << std::endl;
}
