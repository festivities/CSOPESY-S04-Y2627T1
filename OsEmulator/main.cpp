// CSOPESY S04-Y2627T1 - GROUP 6
// This file contains the 'main' function. This holds the main loop
// for the CLI, which maps input to appropriate commands for the OperatingSystem
// and Marquee classes.
//

#include <cstdlib>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include "Marquee.h"
#include "OperatingSystem.h"

int main() {
    OperatingSystem os;
    os.printMainMenu();

	// command input loop
    while (true) {
		std::string command;

		// prompt user input (lock console while doing so)
		{
			std::lock_guard<std::mutex> lk(Marquee::ConsoleMutex());
			std::cout << "root:\\> ";
		}
		// exit the program if input fails or reaches end of input
		if (!std::getline(std::cin, command)) {
			return 0;
		}

		command = trim(command); // trim whitespace

		// before initialize only "initialize" and "exit" are recognized
		if (command != "exit" && command != "initialize" && !os.isInitialized()) {
			std::cout << "Please run \"initialize\" first." << std::endl;
			continue;
		}

		if (command == "exit") {
			// immediately exit (destructors stop the marquee and scheduler threads)
			return 0;
		} else if (command == "clear") {
			//  clear the console
			os.clearScreen();
		}
		else if (command == "initialize") {
			// read config.txt and start the operating system
			os.initialize();
		} else if (command == "screen" || command.starts_with("screen ")) {
			// screen -ls, screen -s <name> or screen -r <name>
			std::istringstream args(command.substr(6));
			std::string flag;
			std::string name;
			std::string extra;
			args >> flag >> name;

			// -ls takes no name, -s and -r take exactly one name
			bool validName = !name.empty()
				&& name.find_first_of(" \t\v\r\n") == std::string::npos;
			bool valid = (flag == "-ls" && name.empty())
				|| ((flag == "-s" || flag == "-r") && validName && !(args >> extra));

			if (!valid) {
				std::cout << "Usage: screen -s <name> | screen -r <name> | screen -ls" << std::endl;
			} else if (flag == "-ls") {
				// list running and finished processes
				os.screenList();
			} else if (flag == "-s") {
				// create a process and open its screen
				os.screenCreate(name);
			} else {
				// open the screen of an existing process
				os.screenAttach(name);
			}
		} else if (command == "scheduler-start") {
			// start generating processes
			os.schedulerStart();
		} else if (command == "scheduler-stop") {
			// stop generating processes
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
		else if (command == "set_text" || command.starts_with("set_text ")) {
			// set marquee text

			// extract text and trim whitespace
			std::string t = command.substr(std::string("set_text").size());
			t = trim(t);

			// check if no text provided
			if (t.empty()) {
				std::cout << "Usage: set_text <text>" << std::endl;
				continue;
			}

			// set if valid
			os.setMarqueeText(std::string(t));
		}
		else if (command == "set_speed" || command.starts_with("set_speed ")) {
			// set marquee speed

			std::string sp = trim(command.substr(std::string("set_speed").size()));

			// check if speed is empty
			if (sp.empty()) {
				std::cout << "Usage: set_speed <speed>" << std::endl;
				continue;
			}

			// convert speed to integer and set it
			try {
				std::size_t parsed = 0;
				long long speedValue = std::stoll(sp, &parsed);
				if (parsed != sp.size()
					|| speedValue <= 0
					|| speedValue > std::numeric_limits<int>::max()) {
					throw std::invalid_argument("invalid marquee speed");
				}
				int speed = static_cast<int>(speedValue);
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
