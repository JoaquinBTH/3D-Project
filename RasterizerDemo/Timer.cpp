#include "Timer.h"

Timer::Timer()
{
	start = std::chrono::high_resolution_clock::now();
}

double Timer::GetElapsedTime()
{
	auto elapsedTime = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - start);
	return elapsedTime.count();
}

void Timer::Restart()
{
	start = std::chrono::high_resolution_clock::now();
}
