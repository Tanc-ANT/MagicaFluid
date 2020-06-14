#include "ParticleSystemData.h"

ParticleSystemData::ParticleSystemData()
{
}

ParticleSystemData::ParticleSystemData(size_t numberOfParticles)
{
	resize(numberOfParticles);
}

ParticleSystemData::~ParticleSystemData()
{
}

void ParticleSystemData::resize(size_t newNumberOfParticles)
{
	_numberOfParticles = newNumberOfParticles;

	_positions.resize(newNumberOfParticles, Vector3D());
	_velocities.resize(newNumberOfParticles, Vector3D());
	_forces.resize(newNumberOfParticles, Vector3D());

}

size_t ParticleSystemData::numberOfParticles() const
{
	return _numberOfParticles;
}

VectorArray& ParticleSystemData::positions()
{
	return _positions;
}

VectorArray& ParticleSystemData::velocities()
{
	return _velocities;
}

VectorArray& ParticleSystemData::forces()
{
	return _forces;
}

double ParticleSystemData::radius() const
{
	return _radius;
}

void ParticleSystemData::setRadius(double newRadius) {
	_radius = std::max(newRadius, 0.0);
}

double ParticleSystemData::mass() const
{
	return _mass;
}

void ParticleSystemData::setMass(double newMass) {
	_mass = std::max(newMass, 0.0);
}

void ParticleSystemData::addParticle(const Vector3D& newPosition,const Vector3D& newVelocity,const Vector3D& newForce)
{
	VectorArray newPositions = { newPosition };
	VectorArray newVelocities = { newVelocity };
	VectorArray newForces = { newForce };

	addParticles(newPositions,newVelocities,newForces);
}

void ParticleSystemData::addParticles(const VectorArray& newPositions,const VectorArray& newVelocities,	const VectorArray& newForces)
{
	if (newVelocities.size() > 0 && newVelocities.size() != newPositions.size())
	{
		std::cout << "newVelocities.size() > 0 && newVelocities.size() != newPositions.size()" << std::endl;
	}
	if (newForces.size() > 0 && newForces.size() != newPositions.size())
	{
		std::cout << "newForces.size() > 0 && newForces.size() != newPositions.size()" << std::endl;
	}

	size_t oldNumberOfParticles = numberOfParticles();
	size_t newNumberOfParticles = oldNumberOfParticles + newPositions.size();

	resize(newNumberOfParticles);

	auto& pos = positions();
	auto& vel = velocities();
	auto& frc = forces();

	for (int i = 0; i < newPositions.size();++i)
	{
		pos[i + oldNumberOfParticles] = newPositions[i];
	}

	for (int i = 0; i < newVelocities.size(); ++i)
	{
		vel[i + oldNumberOfParticles] = newVelocities[i];
	}

	for (int i = 0; i < newForces.size(); ++i)
	{
		frc[i + oldNumberOfParticles] = newForces[i];
	}
}

const NeighborSearcherPtr& ParticleSystemData::neighborSearcher() const
{
	return _neighborSearcher;
}

void ParticleSystemData::buildNeighborSearcher(double maxSearchRadius)
{
	_neighborSearcher = std::make_shared<NeighborSearcher>(8,8,8,2.0 * maxSearchRadius);

	_neighborSearcher->build(positions());
}

const std::vector<std::vector<size_t>>& ParticleSystemData::neighborLists() const
{
	return _neighborLists;
}

void ParticleSystemData::buildNeighborLists(double maxSearchRadius)
{
	_neighborLists.resize(numberOfParticles());

	const auto& points = positions();
	for (size_t i = 0; i < numberOfParticles(); ++i) {
		Vector3D origin = points[i];
		_neighborLists[i].clear();

		_neighborSearcher->forEachNearbyPoint(
			origin,
			maxSearchRadius,
			[&](size_t j, const Vector3D&) {
			if (i != j) {
				_neighborLists[i].push_back(j);
			}
		});
	}
}