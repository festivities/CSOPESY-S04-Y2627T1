// CSOPESY S04-Y2627T1 - GROUP 6
// This header file contains the declaration of the OperatingSystem class.
//

#pragma once

#include "Marquee.h"
#include <iostream>
#include <string>
#include <map>

class OperatingSystem {
private:
	// supported commands and their descriptions
	std::map<std::string, std::string> commands;
	Marquee marquee;
public:

	// Constructor
	OperatingSystem();

	// Getters
	std::map<std::string, std::string> getCommands() const;

	// Commands
	void initialize() const;
	void screen() const;
	void schedulerStart() const;
	void schedulerStop() const;
	void reportUtil() const;
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
};