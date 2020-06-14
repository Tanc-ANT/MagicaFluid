#include "pch.h"
#include "type.h"
#include "PhysicsAnimation.h"


PhysicsAnimation::PhysicsAnimation()
{
}

PhysicsAnimation::~PhysicsAnimation()
{
}

void PhysicsAnimation::update(const Frame& frame)
{
	if (!_runing)
	{
		return;
	}
	onUpdate(frame);
}

bool PhysicsAnimation::isUsingFixedSubTimeSteps() const {
	return _isUsingFixedSubTimeSteps;
}

void PhysicsAnimation::setIsUsingFixedSubTimeSteps(bool isUsing)
{
	_isUsingFixedSubTimeSteps = isUsing;
}

unsigned int PhysicsAnimation::numberOfFixedSubTimeSteps() const
{
	return _numberOfFixedSubTimeSteps;
}

void PhysicsAnimation::setNumberOfFixedSubTimeSteps(unsigned int numberOfSteps)
{
	_numberOfFixedSubTimeSteps = numberOfSteps;
}

void PhysicsAnimation::advanceSingleFrame()
{
	++_currentFrame;
	update(_currentFrame);
}

Frame PhysicsAnimation::currentFrame() const
{
	return _currentFrame;
}

void PhysicsAnimation::setCurrentFrame(const Frame& frame)
{
	_currentFrame = frame;
}

double PhysicsAnimation::currentTimeInSeconds() const
{
	return _currentTime;
}

unsigned int PhysicsAnimation::numberOfSubTimeSteps(double timeIntervalInSeconds) const
{
	// Returns number of fixed sub-timesteps by default
	return _numberOfFixedSubTimeSteps;
}

bool PhysicsAnimation::isRuning() const
{
	return _runing;
}

bool PhysicsAnimation::setRuing(bool isRuning)
{
	return _runing = isRuning;
}

void PhysicsAnimation::onUpdate(const Frame& frame) 
{
	/*if (frame.index > _currentFrame.index)
	{
		unsigned int numberOfFrames = frame.index - _currentFrame.index;

		for (unsigned int i = 0; i < numberOfFrames; ++i) {
			advanceTimeStep(frame.timeIntervalInSeconds);
		}

		_currentFrame = frame;
	}
	else if (frame.index == 0 && !_hasInitialized)
	{
		initialize();
	}*/

	advanceTimeStep(frame.timeIntervalInSeconds);
	_currentFrame = frame;
}

void PhysicsAnimation::advanceTimeStep(double timeIntervalInSeconds)
{
	_currentTime = _currentFrame.timeInSeconds();

	if (_isUsingFixedSubTimeSteps) {
		/*std::cout << "Using fixed sub-timesteps: " << _numberOfFixedSubTimeSteps << std::endl;*/

		// Perform fixed time-stepping
		const double actualTimeInterval = timeIntervalInSeconds / static_cast<double>(_numberOfFixedSubTimeSteps);

		for (unsigned int i = 0; i < _numberOfFixedSubTimeSteps; ++i)
		{
			/*std::cout << "Begin onAdvanceTimeStep: " << actualTimeInterval
				<< " (1/" << 1.0 / actualTimeInterval
				<< ") seconds" << std::endl;*/

			onAdvanceTimeStep(actualTimeInterval);

			_currentTime += actualTimeInterval;
		}
	}
	else {
		std::cout<< "Using adaptive sub-timesteps"<<std::endl;

		// Perform adaptive time-stepping
		double remainingTime = timeIntervalInSeconds;
		while (remainingTime > kEpsilonD)
		{
			unsigned int numSteps = numberOfSubTimeSteps(remainingTime);
			double actualTimeInterval
				= remainingTime / static_cast<double>(numSteps);

			std::cout << "Number of remaining sub-timesteps: " << numSteps <<std::endl;

			std::cout << "Begin onAdvanceTimeStep: " << actualTimeInterval
				<< " (1/" << 1.0 / actualTimeInterval
				<< ") seconds"<<std::endl;

			onAdvanceTimeStep(actualTimeInterval);

			remainingTime -= actualTimeInterval;
			_currentTime += actualTimeInterval;
		}
	}
}

void PhysicsAnimation::initialize() 
{
	onInitialize();
	_hasInitialized = true;
}

void PhysicsAnimation::onInitialize()
{
	// Do nothing
}