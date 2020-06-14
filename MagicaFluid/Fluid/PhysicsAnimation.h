#pragma once
#include "Frame.h"

class PhysicsAnimation
{
public:
	PhysicsAnimation();
	virtual ~PhysicsAnimation();

	void initialize();

	void update(const Frame& frame);

	bool isUsingFixedSubTimeSteps() const;

	void setIsUsingFixedSubTimeSteps(bool isUsing);

	unsigned int numberOfFixedSubTimeSteps() const;

	void setNumberOfFixedSubTimeSteps(unsigned int numberOfSteps);

	void advanceSingleFrame();

	Frame currentFrame() const;

	void setCurrentFrame(const Frame& frame);

	double currentTimeInSeconds() const;

	bool isRuning() const;

	bool setRuing(bool isRuning);

protected:
	virtual void onAdvanceTimeStep(double timeIntervalInSeconds) = 0;

	virtual unsigned int numberOfSubTimeSteps(double timeIntervalInSeconds) const;
	
	virtual void onInitialize();

private:
	Frame _currentFrame;
	bool _isUsingFixedSubTimeSteps = true;
	unsigned int _numberOfFixedSubTimeSteps = 1;
	bool _hasInitialized = false;
	double _currentTime = 0.0;
	bool _runing = false;

	void onUpdate(const Frame& frame);

	void advanceTimeStep(double timeIntervalInSeconds);
};