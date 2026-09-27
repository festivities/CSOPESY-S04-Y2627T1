// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "OperatingSystem.h"
#include <cstdlib>
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
		// exit the program if input fails or reaches end of input
		if (!std::getline(std::cin, command)) {
			return 0;
		}

		if (command == "exit") {
			// immediately exit 
			os.stopMarquee();
			return 0;
		} else if (command == "clear") {
			//  clear the console using cls
			std::system("cls");
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
		}
		else if (command == "help") {
			// display available commands
			os.printCommands();
		}
		else if (command == "start_marquee") {
			// start marquee animation
			os.startMarquee();
		}
		else if (command == "stop_marquee") {
			// stop marquee animation
			os.stopMarquee();
		}
		else if (command.rfind("set_text ", 0) == 0) {
			// set marquee text
			std::string text = command.substr(9); // extract text after "set_text "
			os.setMarqueeText(text);
		}
		else if (command.rfind("set_speed ", 0) == 0) {
			// set marquee speed
			std::string speedStr = command.substr(10); // extract speed after "set_speed "
			try {
				int speed = std::stoi(speedStr);
				os.setMarqueeSpeed(speed);
			}
			catch (const std::invalid_argument&) {
				std::cout << "Please enter a valid integer." << std::endl;
			}
		}
		else {
			// unrecognized command
			std::cout << "Command \"" << command << "\" not recognized." << std::endl;
		}
	}
	
	return 0;
}