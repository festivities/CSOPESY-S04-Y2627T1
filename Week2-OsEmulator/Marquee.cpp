// CSOPESY S04-Y2627T1 - GROUP 6
// This file contains the implementation of the Marquee class
// which handles the SLIDING LEFT marquee text animation in the console.
//

#include "Marquee.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>

Marquee::Marquee(int width) {
    this->width = width;
    this->text = "Hello world!";
    this->speed = 200;
    this->running = false;
}

Marquee::~Marquee() {
	this->stop();
}

void Marquee::start() {
    // if running already, do nothing
    if (this->running.load()) {
        std::cout << "Marquee is already running." << std::endl;
        return;
    }
    this->running.store(true); // flag true once running

    {
        std::lock_guard<std::mutex> lk(Marquee::ConsoleMutex());
        // \033[2J clears screen
        // \033[2;1H moves cursor to row 2, col 1
        std::cout << "\033[2J\033[2;1H" << std::flush;
    }

    // start animation thread
    // https://en.cppreference.com/w/cpp/thread/thread/thread
    this->animationThread = std::thread(&Marquee::animate, this);
}

void Marquee::stop() {
    if (!this->running.load()) return;
    this->running.store(false);

    // https://en.cppreference.com/w/cpp/thread/thread/join
    if (this->animationThread.joinable()) {
        this->animationThread.join();
    }

	// clear top row (where marquee was drawn) after stopping
    {
        std::lock_guard<std::mutex> lk(Marquee::ConsoleMutex());
        // \033[s saves cursor
        // \033[1;1H jumps to top
        // \033[2K clears line
        // \033[u restores cursor
        std::cout << "\033[s\033[1;1H\033[2K\033[u" << std::flush;
    }
}

bool Marquee::getRunning() const {
	return this->running.load();
}

void Marquee::setText(const std::string& text) {
    std::lock_guard<std::mutex> lk(Marquee::ConsoleMutex());
    this->text = text;
}

void Marquee::setSpeed(int speed) {
    this->speed = speed;
}

void Marquee::animate() {
    int pos = this->width;

    
    while (this->running.load()) {
        std::string line(this->width, ' ');

        {
            std::lock_guard<std::mutex> lk(Marquee::ConsoleMutex());
            for (size_t i = 0; i < this->text.size(); ++i) {
                int p = pos + static_cast<int>(i);
                if (p >= 0 && p < this->width) {
                    line[p] = this->text[i];
                }
            }
        }

        std::string framed = "[" + line + "]";

        {
            std::lock_guard<std::mutex> lk(Marquee::ConsoleMutex());
            std::cout << "\033[s"     // save where the user is currently typing
                << "\033[1;1H"        // move cursor straight to row 1, col 1
                << "\033[2K"          // wipe old marquee completely
                << framed             // print the new marquee frame
                << "\033[u"           // snap cursor back to prev typing position
                << std::flush;        // force immediate render to terminal screen
        }

        // advance the text position (scroll left)
        pos--;
        if (pos < -static_cast<int>(this->text.size())) {
            pos = this->width;
        }

		// sleep for the specified speed duration
        // https://en.cppreference.com/w/cpp/thread/sleep_for
        std::this_thread::sleep_for(std::chrono::milliseconds(this->speed));
    }
}
