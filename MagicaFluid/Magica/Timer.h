#pragma once

#include <chrono>

class Timer
{
private:
	int frames = 0;
	double FPS = 0;
	std::chrono::time_point<std::chrono::steady_clock> oldTime = std::chrono::high_resolution_clock::now();

public:
	Timer();
	void update();
	double get_fps();
};
