// CSOPESY S04-Y2627T1 - GROUP 6
// This file contains the 'main' function. This holds the main loop
// for the CLI, which maps input to appropriate commands for the OperatingSystem
// and Marquee classes.
//

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include "Marquee.h"
#include "OperatingSystem.h"

// Helper to remove leading and trailing whitespace from string
std::string trim(const std::string& text)
{
	const char* WhiteSpace = " \t\v\r\n";
	std::size_t start = text.find_first_not_of(WhiteSpace);
	std::size_t end = text.find_last_not_of(WhiteSpace);
	return start == std::string::npos ? std::string() : text.substr(start, end - start + 1);
}

int main() {
    OperatingSystem os;
    os.printMainMenu();

	// command input loop
    while (true) {
		std::string command;
		
		// prompt user input (lock console while doing so)
		{
			std::lock_guard<std::mutex> lk(Marquee::ConsoleMutex());
			std::cout << "Command>";
		}
		// exit the program if input fails or reaches end of input
		if (!std::getline(std::cin, command)) {
			return 0;
		}

		command = trim(command); // trim whitespace

		if (command == "exit") {
			// immediately exit (stop marquee if running
			os.stopMarquee();
			return 0;
		} else if (command == "clear") {
			//  clear the console
			os.clearScreen();
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
		else if (command.starts_with("set_text")) {
			// set marquee text
			
			// if no text provided
			if (command.length() <= 9) {
				std::cout << "Usage: set_text <text>" << std::endl;
				continue;
			} 

			// extract text and trim whitespace
			std::string t = command.substr(9);
			t = trim(t);

			// check if no text provided
			if (t.empty()) {
				std::cout << "Usage: set_text <text>" << std::endl;
				continue;
			}

			// set if valid
			os.setMarqueeText(std::string(t));
		}
		else if (command.starts_with("set_speed")) {
			// set marquee speed
			
			// if no text provided
			if (command.length() <= 10) {
				std::cout << "Usage: set_speed <speed>" << std::endl;
				continue;
			}

			std::string sp = command.substr(10);

			// check if speed is empty
			if (sp.empty()) {
				std::cout << "Usage: set_speed <speed>" << std::endl;
				continue;
			}

			// convert speed to integer and set it
			try {
				int speed = std::stoi(trim(sp));
				os.setMarqueeSpeed(speed);
			}
			catch (const std::invalid_argument&) {
				// not convertable to integer
				std::cout << "Provided speed is not a valid integer." << std::endl;
			}
			catch (const std::out_of_range&) {
				std::cout << "Provided speed is not a valid integer." << std::endl;
			}
		}
		else {
			// unrecognized command
			std::cout << "Command \"" << command << "\" not recognized." << std::endl;
		}
	}
	
	return 0;
}
