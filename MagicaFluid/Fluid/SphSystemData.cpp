#include "SphSystemData.h"
#include "BoundingBox.h"
#include "PointGenerator.h"

SphSystemData::SphSystemData() : SphSystemData(0)
{
}

SphSystemData::SphSystemData(size_t numberOfParticles)
	: ParticleSystemData(numberOfParticles)
{
	_density.resize(numberOfParticles);
	_pressure.resize(numberOfParticles);

	setTargetSpacing(_targetSpacing);
}

SphSystemData::~SphSystemData()
{
}

void SphSystemData::resize(size_t newNumberOfParticles)
{
	ParticleSystemData::resize(newNumberOfParticles);
	_pressure.resize(newNumberOfParticles);
	_density.resize(newNumberOfParticles);
	computeMass();
}

void SphSystemData::setRadius(double newRadius) 
{
	// Interprete it as setting target spacing
	setTargetSpacing(newRadius);
}

void SphSystemData::setMass(double newMass)
{
	double incRatio = newMass / mass();
	_targetDensity *= incRatio;
	ParticleSystemData::setMass(newMass);
}

std::vector<double>& SphSystemData::densities()
{
	return _density;
}

std::vector<double>& SphSystemData::pressures()
{
	return _pressure;
}

void SphSystemData::updateDensities() {
	const auto& p = positions();
	auto& d = densities();
	const double m = mass();

	for (int i = 0; i < numberOfParticles(); ++i)
	{
		double sum = sumOfKernelNearby(p[i]);
		d[i] = m * sum;
	}
}

void SphSystemData::setTargetDensity(double targetDensity)
{
	_targetDensity = targetDensity;

	computeMass();
}

double SphSystemData::targetDensity() const
{
	return _targetDensity;
}

void SphSystemData::setTargetSpacing(double spacing)
{
	ParticleSystemData::setRadius(spacing);

	_targetSpacing = spacing;
	_kernelRadius = _kernelRadiusOverTargetSpacing * _targetSpacing;

	computeMass();
}

double SphSystemData::targetSpacing() const
{
	return _targetSpacing;
}

void SphSystemData::setRelativeKernelRadius(double relativeRadius)
{
	_kernelRadiusOverTargetSpacing = relativeRadius;
	_kernelRadius = _kernelRadiusOverTargetSpacing * _targetSpacing;

	computeMass();
}

double SphSystemData::relativeKernelRadius() const
{
	return _kernelRadiusOverTargetSpacing;
}

double SphSystemData::kernelRadius() const
{
	return _kernelRadius;
}

double SphSystemData::sumOfKernelNearby(const Vector3D& origin)
{
	double sum = 0.0;
	SphStdKernel kernel(_kernelRadius);
	neighborSearcher()->forEachNearbyPoint(
		origin,
		_kernelRadius,
		[&](size_t, const Vector3D& neighborPosition)
	{
		auto distance = origin - neighborPosition;
		double dist = std::sqrt(
			distance.x * distance.x +
			distance.y * distance.y +
			distance.z * distance.z);
		sum += kernel(dist);
	});
	return sum;
}

double SphSystemData::interpolate(
	const Vector3D& origin,
	const std::vector<double>& values) 
{
	double sum = 0.0;
	const auto& d = densities();
	SphStdKernel kernel(_kernelRadius);
	const double m = mass();

	neighborSearcher()->forEachNearbyPoint(
		origin,
		_kernelRadius,
		[&](size_t i, const Vector3D& neighborPosition)
	{
		auto distance = origin - neighborPosition;
		double dist = std::sqrt(
			distance.x * distance.x +
			distance.y * distance.y +
			distance.z * distance.z);
		double weight = m / d[i] * kernel(dist);
		sum += weight * values[i];
	});

	return sum;
}

Vector3D SphSystemData::interpolate(
	const Vector3D& origin,
	const VectorArray& values)
{
	Vector3D sum(0,0,0);
	const auto d = densities();
	SphStdKernel kernel(_kernelRadius);
	const double m = mass();

	neighborSearcher()->forEachNearbyPoint(
		origin,
		_kernelRadius,
		[&](size_t i, const Vector3D& neighborPosition)
	{
		auto distance = origin - neighborPosition;
		double dist = std::sqrt(
			distance.x * distance.x +
			distance.y * distance.y +
			distance.z * distance.z);
		double weight = m / d[i] * kernel(dist);
		sum += weight * values[i];
	});

	return sum;
}

Vector3D SphSystemData::gradientAt(
	size_t i,
	const std::vector<double>& values) 
{
	Vector3D sum(0, 0, 0);
	const auto& p = positions();
	const auto& d = densities();
	const auto& neighbors = neighborLists()[i];
	Vector3D origin = p[i];
	SphStdKernel kernel(_kernelRadius);
	const double m = mass();

	for (size_t j : neighbors)
	{
		Vector3D neighborPosition = p[j];
		auto distance = origin - neighborPosition;
		double dist = std::sqrt(
			distance.x * distance.x +
			distance.y * distance.y +
			distance.z * distance.z);
		if (dist > 0.0) {
			Vector3D dir = (neighborPosition - origin) / dist;
			sum
				+= d[i] * m
				* (values[i] / std::pow(d[i],2) + values[j] / std::pow(d[j],2))
				* kernel.gradient(dist, dir);
		}
	}

	return sum;
}

double SphSystemData::laplacianAt(
	size_t i,
	const std::vector<double>& values)
{
	double sum = 0.0;
	const auto& p = positions();
	const auto& d = densities();
	const auto& neighbors = neighborLists()[i];
	Vector3D origin = p[i];
	SphStdKernel kernel(_kernelRadius);
	const double m = mass();

	for (size_t j : neighbors) {
		Vector3D neighborPosition = p[j];
		auto distance = origin - neighborPosition;
		double dist = std::sqrt(
			distance.x * distance.x +
			distance.y * distance.y +
			distance.z * distance.z);
		sum +=
			m * (values[j] - values[i]) / d[j] * kernel.secondDerivative(dist);
	}

	return sum;
}

Vector3D SphSystemData::laplacianAt(
	size_t i,
	const VectorArray& values)
{
	Vector3D sum(0,0,0);
	auto p = positions();
	auto d = densities();
	const auto& neighbors = neighborLists()[i];
	Vector3D origin = p[i];
	SphStdKernel kernel(_kernelRadius);
	const double m = mass();

	for (size_t j : neighbors) {
		Vector3D neighborPosition = p[j];
		auto distance = origin - neighborPosition;
		double dist = std::sqrt(
			distance.x * distance.x +
			distance.y * distance.y +
			distance.z * distance.z);
		sum +=
			m * (values[j] - values[i]) / d[j] * kernel.secondDerivative(dist);
	}

	return sum;
}

void SphSystemData::buildNeighborSearcher()
{
	ParticleSystemData::buildNeighborSearcher(_kernelRadius);
}

void SphSystemData::buildNeighborLists()
{
	ParticleSystemData::buildNeighborLists(_kernelRadius);
}


void SphSystemData::computeMass()
{
	VectorArray points;
	BccLatticePointGenerator pointsGenerator;

	BoundingBox sampleBound(
		Vector3D(-1.5 * _kernelRadius, -1.5 * _kernelRadius, -1.5 * _kernelRadius),
		Vector3D(1.5 * _kernelRadius, 1.5 * _kernelRadius, 1.5 * _kernelRadius));

	pointsGenerator.generate(sampleBound, _targetSpacing, points);

	double maxNumberDensity = 0.0;
	SphStdKernel kernel(_kernelRadius);

	for (size_t i = 0; i < points.size(); ++i) {
		const Vector3D& point = points[i];
		double sum = 0.0;

		for (size_t j = 0; j < points.size(); ++j) {
			const Vector3D& neighborPoint = points[j];
			auto distance = neighborPoint - point;
			double dist = std::sqrt(
				distance.x * distance.x +
				distance.y * distance.y +
				distance.z * distance.z);
			sum += kernel(dist);
		}

		maxNumberDensity = std::max(maxNumberDensity, sum);
	}

	assert(maxNumberDensity > 0);

	double newMass = _targetDensity / maxNumberDensity;

	ParticleSystemData::setMass(newMass);
}