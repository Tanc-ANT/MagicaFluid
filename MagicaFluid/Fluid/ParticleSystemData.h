#pragma once
#include "pch.h"
#include "type.h"
#include "NeighborSearcher.h"

class ParticleSystemData
{
public:
	ParticleSystemData();
	virtual ~ParticleSystemData();

	explicit ParticleSystemData(size_t numberOfParticles);

	// It's very important which differ from the original.
	virtual void resize(size_t newNumberOfParticles);

	size_t numberOfParticles() const;

	VectorArray& positions();
	VectorArray& velocities();
	VectorArray& forces();

	double radius() const;
	virtual void setRadius(double newRadius);
	double mass() const;
	virtual void setMass(double newMass);

	void addParticle(
		const Vector3D& newPosition,
		const Vector3D& newVelocity = Vector3D(),
		const Vector3D& newForce = Vector3D());

	void addParticles(
		const VectorArray& newPositions,
		const VectorArray& newVelocities = VectorArray(),
		const VectorArray& newForces = VectorArray());

	const NeighborSearcherPtr& neighborSearcher() const;
	void buildNeighborSearcher(double maxSearchRadius);

	const std::vector<std::vector<size_t>>& neighborLists() const;
	void buildNeighborLists(double maxSearchRadius);

private:
	double _radius = 0.04;
	double _mass = 0.5;
	size_t _numberOfParticles = 0;

	VectorArray _positions;
	VectorArray _velocities;
	VectorArray _forces;

	NeighborSearcherPtr _neighborSearcher;
	std::vector<std::vector<size_t>> _neighborLists;
};

typedef std::shared_ptr<ParticleSystemData> ParticleSystemDataPtr;