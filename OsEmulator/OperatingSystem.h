// CSOPESY S04-Y2627T1 - GROUP 6
// This header file contains the declaration of the OperatingSystem class.
//

#pragma once

#include "Marquee.h"
#include "Process.h"
#include "Scheduler.h"
#include <iostream>
#include <memory>
#include <string>
#include <map>

// Helper to remove leading and trailing whitespace from string
std::string trim(const std::string& text);

class OperatingSystem {
private:
	// supported commands and their descriptions
	std::map<std::string, std::string> commands;
	Marquee marquee;
	Scheduler scheduler;
	// true after a successful initialize
	bool initialized;

	// process screen: process-smi, exit (back to the main menu)
	void processScreen(const std::shared_ptr<Process>& proc);
public:

	// Constructor
	OperatingSystem();

	// Getters
	std::map<std::string, std::string> getCommands() const;
	bool isInitialized() const;

	// Commands
	void initialize();
	void screenCreate(const std::string& name);
	void screenAttach(const std::string& name);
	void schedulerStart();
	void schedulerStop();
	void reportUtil();
	void clearScreen() const;
	// exit handled in main

	// print menu and commands
	void printMainMenu() const;
	void printCommands() const;

	// Marquee control
	void startMarquee();
	void stopMarquee();
	void setMarqueeText(const std::string& text);
	void setMarqueeSpeed(int speed);

	// Scheduler control
	void screenList();
};
