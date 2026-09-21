// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "OperatingSystem.h"
#include <iostream>
#include <string>

int main() {
    OperatingSystem os;
    os.printMainMenu();

	// command input loop
    while (true) {
		std::string command;
		
		// prompt user input
		std::cout << "> Enter a command: ";
		std::getline(std::cin, command);

		if (command == "exit") {
			// immediately exit program
			return 0;
		} else if (command == "clear") {
			//  clear based on https://stackoverflow.com/questions/17335816/clear-screen-using-c
			std::cout << "\033[2J\033[1;1H";
			os.printMainMenu();
		}
		else if (command == "initialize") {
			// initialize the operating system
			os.initialize();
		} else if (command == "screen") {
			// display screen information
			os.screen();
		} else if (command == "scheduler-start") {
			// start the scheduler
			os.schedulerStart();
		} else if (command == "scheduler-stop") {
			// stop the scheduler
			os.schedulerStop();
		} else if (command == "report-util") {
			// report utilization
			os.reportUtil();
		} else {
			// unrecognized command
			std::cout << "Command \"" << command << "\" not recognized." << std::endl;
		}
	}
	
	return 0;
}