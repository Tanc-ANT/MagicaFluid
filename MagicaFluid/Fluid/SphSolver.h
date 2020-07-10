#pragma once
#include "ParticleSystemSolver.h"
#include "SphSystemData.h"

class SphSolver : public ParticleSystemSolver
{
public:
	SphSolver();
	SphSolver(
		double targetDensity,
		double targetSpacing,
		double relativeKernelRadius);
	virtual ~SphSolver();

	double eosExponent() const;
	void setEosExponent(double newEosExponent);

	double negativePressureScale() const;
	void setNegativePressureScale(double newNegativePressureScale);

	double viscosityCoefficient() const;
	void setViscosityCoefficient(double newViscosityCoefficient);

	double pseudoViscosityCoefficient() const;
	void setPseudoViscosityCoefficient(double newPseudoViscosityCoefficient);

	double speedOfSound() const;
	void setSpeedOfSound(double newSpeedOfSound);

	double timeStepLimitScale() const;
	void setTimeStepLimitScale(double newScale);

	SphSystemDataPtr sphSystemData() const;


protected:
	unsigned int numberOfSubTimeSteps(double timeIntervalInSeconds) const override;

	void accumulateForces(double timeStepInSeconds) override;

	void onBeginAdvanceTimeStep(double timeStepInSeconds) override;

	void onEndAdvanceTimeStep(double timeStepInSeconds) override;

	virtual void accumulateNonPressureForces(double timeStepInSeconds);

	virtual void accumulatePressureForce(double timeStepInSeconds);

	void computePressure();

	double computePressureFromEos(
		const double& density,
		const double& targetDensity,
		const double& eosScale,
		const double& eosExponent,
		const double& negativePressureScale);

	void accumulatePressureForce(
		const VectorArray& positions,
		const std::vector<double>& densities,
		const std::vector<double>& pressures,
		VectorArray& pressureForces);

	void accumulateViscosityForce();

	void computePseudoViscosity(double timeStepInSeconds);

private:
	double _eosExponent = 1.0;
	double _negativePressureScale = 0;
	double _viscosityCoefficient = 0.025;
	double _pseudoViscosityCoefficient = 0.0;
	double _speedOfSound = 100.0;
	double _timeStepLimitScale = 1.0;
};

typedef std::shared_ptr<SphSolver> SphSolverPtr;

