#include "Marquee.h"
#include <iostream>
#include <chrono>

Marquee::Marquee(int width, int height) {
	this->width = width;
	this->height = height;
	this->text = "Hello world!";
	this->speed = 200;
	this->running = false;
}

Marquee::~Marquee() {
	this->stop();
}

void Marquee::start() {
	if (this->running) {
		std::cout << "Marquee is already running." << std::endl;
		return;
	}
	this->running = true;

	// https://en.cppreference.com/w/cpp/thread/thread/thread
	this->animationThread = std::thread(&Marquee::animate, this);
	std::cout << "Marquee started." << std::endl;
}

// stop marquee animation and wait for the thread to finish
void Marquee::stop() {
	if (!this->running) {
		return;
	}
	this->running = false;

	// https://en.cppreference.com/w/cpp/thread/thread/join
	if (this->animationThread.joinable()) {
		this->animationThread.join();
	}
	std::cout << "Marquee stopped." << std::endl;
}

void Marquee::setText(const std::string& text) {
	this->text = text;
}

// marquee refresh interval in milliseconds
void Marquee::setSpeed(int speed) {
	this->speed = speed;
}

// animation loop that scrolls text from right to left
void Marquee::animate() {
	int pos = this->width;
	while (this->running) {
		// build the visible marquee line
		std::string line(this->width, ' ');
		for (size_t i = 0; i < this->text.length(); i++) {
			int displayPos = pos + static_cast<int>(i);
			if (displayPos >= 0 && displayPos < this->width) {
				line[displayPos] = this->text[i];
			}
		}

		std::cout << "\r[" << line << "]" << std::flush;

		// advance the text position (scroll left)
		pos--;
		if (pos < -static_cast<int>(this->text.length())) {
			pos = this->width;
		}

		// https://en.cppreference.com/w/cpp/thread/sleep_for
		std::this_thread::sleep_for(std::chrono::milliseconds(this->speed));
	}

	// clear the marquee line when stopped
	std::cout << "\r" << std::string(this->width + 2, ' ') << "\r" << std::flush;
}
