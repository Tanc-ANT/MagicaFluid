#include "Timer.h"

Timer::Timer()
{

}

void Timer::update()
{
	frames++;
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - oldTime);
	if (duration >= std::chrono::milliseconds{ 1000 }) {
		FPS = (double)frames/(double)duration.count() * 1000;
		oldTime = std::chrono::high_resolution_clock::now();
		frames = 0;
	}
}

double Timer::get_fps()
{
	return FPS;
}
