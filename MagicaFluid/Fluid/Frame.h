#pragma once

class Frame
{
public:
	Frame();
	Frame(unsigned int newIndex, double newTimeIntervalInSeconds);

	double timeInSeconds() const;
	void advance();
	void advance(unsigned int delta);
	Frame& operator++();
	Frame operator++(int);

public:
	unsigned int index = 0;

	// SPH algorithm need tiny time interval.
	// PICSPH can have big time interval.
	double timeIntervalInSeconds = 0.001;
};