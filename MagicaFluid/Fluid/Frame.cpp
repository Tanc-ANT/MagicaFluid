#include "Frame.h"

Frame::Frame()
{
}

Frame::Frame(unsigned int newIndex, double newTimeIntervalInSeconds)
	: index(newIndex)
	, timeIntervalInSeconds(newTimeIntervalInSeconds)
{
}

double Frame::timeInSeconds() const
{
	return index * timeIntervalInSeconds;
}

void Frame::advance()
{
	++index;
}

void Frame::advance(unsigned int delta)
{
	index += delta;
}

Frame& Frame::operator++()
{
	advance();
	return *this;
}

Frame Frame::operator++(int i)
{
	Frame result = *this;
	advance();
	return result;
}