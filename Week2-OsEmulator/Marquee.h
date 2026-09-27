#pragma once

#include <string>
#include <thread>
#include <atomic>

class Marquee {
private:
	int width;
	int height;
	std::string text;
	int speed;

	// https://en.cppreference.com/w/cpp/atomic/atomic
	std::atomic<bool> running;

	// https://en.cppreference.com/w/cpp/thread/thread
	std::thread animationThread;

	// animation loop run on separate thread
	void animate();

public:
	// constructor
	Marquee(int width, int height);

	// destructor
	~Marquee();

	// control
	void start();
	void stop();

	// setters
	void setText(const std::string& text);
	void setSpeed(int speed);
};