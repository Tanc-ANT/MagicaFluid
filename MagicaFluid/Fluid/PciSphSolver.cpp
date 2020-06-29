#include "PciSphSolver.h"
#include "PointGenerator.h"
#include "BoundingBox.h"


PciSphSolver::PciSphSolver()
{
}

PciSphSolver::PciSphSolver(
	double targetDensity,
	double targetSpacing,
	double relativeKernelRadius)
	: SphSolver(targetDensity, targetSpacing, relativeKernelRadius)
{
}

PciSphSolver::~PciSphSolver() 
{
}

double PciSphSolver::maxDensityErrorRatio() const 
{
	return _maxDensityErrorRatio;
}

void PciSphSolver::setMaxDensityErrorRatio(double ratio) 
{
	_maxDensityErrorRatio = std::max(ratio, 0.0);
}

unsigned int PciSphSolver::maxNumberOfIterations() const 
{
	return _maxNumberOfIterations;
}

void PciSphSolver::setMaxNumberOfIterations(unsigned int n) 
{
	_maxNumberOfIterations = n;
}

void PciSphSolver::accumulatePressureForce(
	double timeIntervalInSeconds) {
	auto particles = sphSystemData();
	const size_t numberOfParticles = particles->numberOfParticles();
	const double delta = computeDelta(timeIntervalInSeconds);
	const double targetDensity = particles->targetDensity();
	const double mass = particles->mass();

	auto& p = particles->pressures();
	auto& d = particles->densities();
	auto& x = particles->positions();
	auto& v = particles->velocities();
	auto& f = particles->forces();

	// Predicted density ds
	std::vector<double> ds(numberOfParticles, 0.0);

	SphStdKernel kernel(particles->kernelRadius());

	// Initialize buffers
	for (int i = 0; i < numberOfParticles; ++i)
	{
		p[i] = 0.0;
		_pressureForces[i] = Vector3D();
		_densityErrors[i] = 0.0;
		ds[i] = d[i];
	}

	unsigned int maxNumIter = 0;
	double maxDensityError;
	double densityErrorRatio = 0.0;

	for (unsigned int k = 0; k < _maxNumberOfIterations; ++k)
	{
		// Predict velocity and position
		for (int i = 0; i < numberOfParticles; ++i)
		{
			_tempVelocities[i]
				= v[i]
				+ timeIntervalInSeconds / mass
				* (f[i] + _pressureForces[i]);
			_tempPositions[i]
				= x[i] + timeIntervalInSeconds * _tempVelocities[i];
		}
		
		// Resolve collisions
		resolveCollision(
			_tempPositions,
			_tempVelocities);

		// Compute pressure from density error
		for (int i = 0; i < numberOfParticles; ++i)
		{
			double weightSum = 0.0;
			const auto& neighbors = particles->neighborLists()[i];

			for (size_t j : neighbors) {
				auto distance = _tempPositions[i] - _tempPositions[j];
				double dist = std::sqrt(
					distance.x * distance.x +
					distance.y * distance.y +
					distance.z * distance.z);
				weightSum += kernel(dist);
			}
			weightSum += kernel(0);

			double density = mass * weightSum;
			double densityError = (density - targetDensity);
			double pressure = delta * densityError;

			if (pressure < 0.0) {
				pressure *= negativePressureScale();
				densityError *= negativePressureScale();
			}

			p[i] += pressure;
			ds[i] = density;
			_densityErrors[i] = densityError;
		}
		

		// Compute pressure gradient force
		for (auto& item : _pressureForces)
		{
			item = Vector3D();
		}
		
		SphSolver::accumulatePressureForce(
			x, ds, p, _pressureForces);

		// Compute max density error
		maxDensityError = 0.0;
		for (size_t i = 0; i < numberOfParticles; ++i) {

			maxDensityError = maxDensityError * maxDensityError >
				_densityErrors[i] * _densityErrors[i] ?
				maxDensityError : _densityErrors[i];
		}

		densityErrorRatio = maxDensityError / targetDensity;
		maxNumIter = k + 1;

		if (std::fabs(densityErrorRatio) < _maxDensityErrorRatio) {
			break;
		}
	}

#ifdef _DEBUG
    std::cout << "Number of PCI iterations: " << maxNumIter << std::endl;
	std::cout << "Max density error after PCI iteration: " << maxDensityError << std::endl;
#endif // _DEBUG

	// Accumulate pressure force
	for (int i = 0; i < numberOfParticles; ++i)
	{
		f[i] += _pressureForces[i];
	}
}

void PciSphSolver::onBeginAdvanceTimeStep(double timeStepInSeconds)
{
	SphSolver::onBeginAdvanceTimeStep(timeStepInSeconds);

	// Allocate temp buffers
	size_t numberOfParticles = particleSystemData()->numberOfParticles();
	_tempPositions.resize(numberOfParticles);
	_tempVelocities.resize(numberOfParticles);
	_pressureForces.resize(numberOfParticles);
	_densityErrors.resize(numberOfParticles);
}


double PciSphSolver::computeDelta(double timeStepInSeconds)
{
	auto particles = sphSystemData();
	const double kernelRadius = particles->kernelRadius();

	VectorArray points;
	BccLatticePointGenerator pointsGenerator;
	Vector3D origin;
	BoundingBox sampleBound(
		Vector3D(-1.5 * kernelRadius, -1.5 * kernelRadius, -1.5 * kernelRadius),
		Vector3D(1.5 * kernelRadius, 1.5 * kernelRadius, 1.5 * kernelRadius));

	pointsGenerator.generate(sampleBound, particles->targetSpacing(), points);

	SphSpikyKernel kernel(kernelRadius);

	double denom = 0;
	Vector3D denom1;
	double denom2 = 0;

	for (size_t i = 0; i < points.size(); ++i) 
	{
		const Vector3D& point = points[i];
		double distanceSquared =
			point.x * point.x +
			point.y * point.y +
			point.z * point.z;

		if (distanceSquared < kernelRadius * kernelRadius) 
		{
			double distance = std::sqrt(distanceSquared);
			Vector3D direction =
				(distance > 0.0) ? point / distance : Vector3D();

			// grad(Wij)
			Vector3D gradWij = kernel.gradient(distance, direction);
			denom1 += gradWij;
			denom2 += 
				gradWij.x * gradWij.x +
				gradWij.y * gradWij.y +
				gradWij.z * gradWij.z;
		}
	}

	denom += -(denom1.x *denom1.x+
			denom1.y * denom1.y+
			denom1.z * denom1.z)
			- denom2;

	return (std::fabs(denom) > 0.0) ?
		-1 / (computeBeta(timeStepInSeconds) * denom) : 0;
}

double PciSphSolver::computeBeta(double timeStepInSeconds)
{
	auto particles = sphSystemData();
	return 2.0 * std::pow((particles->mass() * timeStepInSeconds
		/ particles->targetDensity()), 2);
}