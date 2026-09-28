// CSOPESY S04-Y2627T1 - GROUP 6
// This header file contains the declaration of the Marquee class.
//

#pragma once

#include <string>
#include <thread>
#include <atomic>
#include <mutex>

class Marquee {
private:
	// width of the marquee
	int width;
	// text to display in the marquee
	std::string text;
	// speed of the marquee animation (in milliseconds)
	int speed;

	// atomic boolean to control running state of marquee
	// https://en.cppreference.com/w/cpp/atomic/atomic
	std::atomic<bool> running;

	// thread for running the animation loop
	// https://en.cppreference.com/w/cpp/thread/thread
	std::thread animationThread;

	// animation loop run on separate thread
	void animate();

public:
	// constructor
	Marquee(int width = 30);

	// destructor (since threads are involved)
	~Marquee();

	// non-copyable (avoid copying threads)
	Marquee(const Marquee&) = delete;
	Marquee& operator=(const Marquee&) = delete;

	// non-movable (avoid moving threads)
	Marquee(Marquee&&) = delete;
	Marquee& operator=(Marquee&&) = delete;

	// control
	void start();
	void stop();

	// Console mutex accessor (static) to synchronize console writes
	static std::mutex& ConsoleMutex() {
		static std::mutex consoleMutex;
		return consoleMutex;
	}

	// getter
	bool getRunning() const;

	// setters
	void setText(const std::string& text);
	void setSpeed(int speed);
};
