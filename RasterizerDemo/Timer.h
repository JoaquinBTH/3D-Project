#pragma once

#include <chrono>

class Timer
{
private:
	std::chrono::time_point<std::chrono::steady_clock> start;
public:
	Timer();

	double GetElapsedTime();
	void Restart();
};

