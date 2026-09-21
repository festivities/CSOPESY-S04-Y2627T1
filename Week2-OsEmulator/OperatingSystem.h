#pragma once

#include <iostream>
#include <string>
#include <map>

class OperatingSystem {
private: 
	// supported commands and their descriptions
	std::map<std::string, std::string> commands;
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
	// exit and clear handled in main

	void printMainMenu() const;
};