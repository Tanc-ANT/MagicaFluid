#include "SphSolver.h"
#include "glm/glm.hpp"

static double kTimeStepLimitBySpeedFactor = 0.4;
static double kTimeStepLimitByForceFactor = 0.25;

SphSolver::SphSolver()
{
	setParticleSystemData(std::make_shared<SphSystemData>());

	//For now we using fixed time here
	setIsUsingFixedSubTimeSteps(true);
}

SphSolver::SphSolver(
	double targetDensity,
	double targetSpacing,
	double relativeKernelRadius)
{
	auto sphParticles = std::make_shared<SphSystemData>();
	setParticleSystemData(sphParticles);
	sphParticles->setTargetDensity(targetDensity);
	sphParticles->setTargetSpacing(targetSpacing);
	sphParticles->setRelativeKernelRadius(relativeKernelRadius);

	//For now we using fixed time here
	setIsUsingFixedSubTimeSteps(true);
}

SphSolver::~SphSolver()
{
}

double SphSolver::eosExponent() const
{
	return _eosExponent;
}

void SphSolver::setEosExponent(double newEosExponent)
{
	_eosExponent = std::max(newEosExponent, 1.0);
}

double SphSolver::negativePressureScale() const
{
	return _negativePressureScale;
}

void SphSolver::setNegativePressureScale(double newNegativePressureScale)
{
	_negativePressureScale = std::clamp(newNegativePressureScale, 0.0, 1.0);
}

double SphSolver::viscosityCoefficient() const
{
	return _viscosityCoefficient;
}

void SphSolver::setViscosityCoefficient(double newViscosityCoefficient)
{
	_viscosityCoefficient = std::max(newViscosityCoefficient, 0.0);
}

double SphSolver::pseudoViscosityCoefficient() const
{
	return _pseudoViscosityCoefficient;
}

void SphSolver::setPseudoViscosityCoefficient(
	double newPseudoViscosityCoefficient)
{
	_pseudoViscosityCoefficient
		= std::max(newPseudoViscosityCoefficient, 0.0);
}

double SphSolver::speedOfSound() const
{
	return _speedOfSound;
}

void SphSolver::setSpeedOfSound(double newSpeedOfSound)
{
	_speedOfSound = std::max(newSpeedOfSound, kEpsilonD);
}

double SphSolver::timeStepLimitScale() const
{
	return _timeStepLimitScale;
}

void SphSolver::setTimeStepLimitScale(double newScale)
{
	_timeStepLimitScale = std::max(newScale, 0.0);
}

SphSystemDataPtr SphSolver::sphSystemData() const
{
	return std::dynamic_pointer_cast<SphSystemData>(particleSystemData());
}

unsigned int SphSolver::numberOfSubTimeSteps(double timeIntervalInSeconds) const
{
	auto particles = sphSystemData();
	size_t numberOfParticles = particles->numberOfParticles();
	auto& f = particles->forces();

	const double kernelRadius = particles->kernelRadius();
	const double mass = particles->mass();

	double maxForceMagnitude = 0.0;

	for (size_t i = 0; i < numberOfParticles; ++i)
	{
		maxForceMagnitude = std::max(maxForceMagnitude, glm::length(f[i]));
	}

	double timeStepLimitBySpeed
		= kTimeStepLimitBySpeedFactor * kernelRadius / _speedOfSound;
	double timeStepLimitByForce
		= kTimeStepLimitByForceFactor
		* std::sqrt(kernelRadius * mass / maxForceMagnitude);

	double desiredTimeStep
		= _timeStepLimitScale
		* std::min(timeStepLimitBySpeed, timeStepLimitByForce);

	return static_cast<unsigned int>(
		std::ceil(timeIntervalInSeconds / desiredTimeStep));
}

void SphSolver::accumulateForces(double timeStepInSeconds)
{
	accumulateNonPressureForces(timeStepInSeconds);
	accumulatePressureForce(timeStepInSeconds);
}

void SphSolver::onBeginAdvanceTimeStep(double timeStepInSeconds)
{
	auto particles = sphSystemData();
	particles->buildNeighborSearcher();
	particles->buildNeighborLists();
	particles->updateDensities();
}

void SphSolver::onEndAdvanceTimeStep(double timeStepInSeconds)
{
	computePseudoViscosity(timeStepInSeconds);

	/*
	auto particles = sphSystemData();
	size_t numberOfParticles = particles->numberOfParticles();
	auto densities = particles->densities();

	double maxDensity = 0.0;
	for (size_t i = 0; i < numberOfParticles; ++i) {
		maxDensity = std::max(maxDensity, densities[i]);
	}
	*/
}

void SphSolver::accumulateNonPressureForces(double timeStepInSeconds)
{
	ParticleSystemSolver::accumulateForces(timeStepInSeconds);
	accumulateViscosityForce();
}

void SphSolver::accumulatePressureForce(double timeStepInSeconds)
{
	auto particles = sphSystemData();
	auto& x = particles->positions();
	auto& d = particles->densities();
	auto& p = particles->pressures();
	auto& f = particles->forces();

	computePressure();
	accumulatePressureForce(x, d, p, f);
}

void SphSolver::computePressure() {
	auto particles = sphSystemData();
	size_t numberOfParticles = particles->numberOfParticles();
	const auto& d = particles->densities();
	auto& p = particles->pressures();

	// See Equation 9 from
	// http://cg.informatik.uni-freiburg.de/publications/2007_SCA_SPH.pdf
	const double targetDensity = particles->targetDensity();
	const double eosScale
		= targetDensity * std::pow(_speedOfSound, 2) / _eosExponent;

	for (int i = 0; i < numberOfParticles; ++i)
	{
		p[i] = computePressureFromEos(
			d[i],
			targetDensity,
			eosScale,
			eosExponent(),
			negativePressureScale());
	}
}

double SphSolver::computePressureFromEos(
	const double& density,
	const double& targetDensity,
	const double& eosScale,
	const double& eosExponent,
	const double& negativePressureScale)
{
	// Equation of state
   // (http://www.ifi.uzh.ch/vmml/publications/pcisph/pcisph.pdf)
	double p = eosScale / eosExponent
		* (std::pow((density / targetDensity), eosExponent) - 1.0);

	// Negative pressure scaling
	if (p < 0) {
		p *= negativePressureScale;
	}

	return p;
}

void SphSolver::accumulatePressureForce(
	const VectorArray& positions,
	const std::vector<double>& densities,
	const std::vector<double>& pressures,
	VectorArray& pressureForces)
{
	auto particles = sphSystemData();
	size_t numberOfParticles = particles->numberOfParticles();

	const double massSquared = std::pow(particles->mass(), 2);
	const SphSpikyKernel kernel(particles->kernelRadius());

	for (int i = 0; i < numberOfParticles; ++i)
	{
		const auto& neighbors = particles->neighborLists()[i];
		for (size_t j : neighbors)
		{
			double dist = glm::distance(positions[i], positions[j]);
			
			if (dist > 0.0) {
				Vector3D dir = (positions[j] - positions[i]) / dist;
				pressureForces[i] -= massSquared
					* (pressures[i] / (densities[i] * densities[i])
						+ pressures[j] / (densities[j] * densities[j]))
					* kernel.gradient(dist, dir);
			}
		}
	}
}

void SphSolver::accumulateViscosityForce()
{
	auto particles = sphSystemData();
	size_t numberOfParticles = particles->numberOfParticles();
	const auto& x = particles->positions();
	const auto& v = particles->velocities();
	const auto& d = particles->densities();
	auto& f = particles->forces();

	const double massSquared = std::pow(particles->mass(), 2);
	const SphViscosityKernel kernel(particles->kernelRadius());

	for (int i = 0; i < numberOfParticles; ++i)
	{
		const auto& neighbors = particles->neighborLists()[i];
		for (size_t j : neighbors)
		{
			double dist = glm::distance(x[i], x[j]);

			f[i] += viscosityCoefficient() * massSquared
				* (v[j] - v[i]) / d[j]
				* kernel.secondDerivative(dist);
		}
	}
}

void SphSolver::computePseudoViscosity(double timeStepInSeconds)
{
	auto particles = sphSystemData();
	size_t numberOfParticles = particles->numberOfParticles();
	const auto& x = particles->positions();
	auto& v = particles->velocities();
	const auto& d = particles->densities();

	const double mass = particles->mass();
	const SphStdKernel kernel(particles->kernelRadius());

	VectorArray smoothedVelocities(numberOfParticles);

	for (int i = 0; i < numberOfParticles; ++i)
	{
		double weightSum = 0.0;
		Vector3D smoothedVelocity;

		const auto& neighbors = particles->neighborLists()[i];
		for (size_t j : neighbors) {
			double dist = glm::distance(x[i], x[j]);

			double wj = mass / d[j] * kernel(dist);
			weightSum += wj;
			smoothedVelocity += wj * v[j];
		}

		double wi = mass / d[i];
		weightSum += wi;
		smoothedVelocity += wi * v[i];

		if (weightSum > 0.0) {
			smoothedVelocity /= weightSum;
		}

		smoothedVelocities[i] = smoothedVelocity;
	}

	double factor = timeStepInSeconds * _pseudoViscosityCoefficient;
	factor = std::clamp(factor, 0.0, 1.0);

	for (int i = 0; i < numberOfParticles; ++i)
	{
		v[i] = (1 - factor) * v[i] + factor * smoothedVelocities[i];
	}
}