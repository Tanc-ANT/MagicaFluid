#pragma once
#include "pch.h"
#include "type.h"
#include "SphSolver.h"

class PciSphSolver : public SphSolver
{
public:
	PciSphSolver();

	//! Constructs a solver with target density, spacing, and relative kernel
	//! radius.
	PciSphSolver(
		double targetDensity,
		double targetSpacing,
		double relativeKernelRadius);

	virtual ~PciSphSolver();

	//! Returns max allowed density error ratio.
	double maxDensityErrorRatio() const;
	void setMaxDensityErrorRatio(double ratio);

	unsigned int maxNumberOfIterations() const;
	void setMaxNumberOfIterations(unsigned int n);

protected:
	void accumulatePressureForce(double timeIntervalInSeconds) override;

	void onBeginAdvanceTimeStep(double timeStepInSeconds) override;

private:
	double _maxDensityErrorRatio = 0.01;
	unsigned int _maxNumberOfIterations = 10;
	
	VectorArray _tempPositions;
	VectorArray _tempVelocities;
	VectorArray _pressureForces;
	std::vector<double> _densityErrors;

	double computeDelta(double timeStepInSeconds);
	double computeBeta(double timeStepInSeconds);

};

typedef std::shared_ptr<PciSphSolver> PciSphSolverPtr;