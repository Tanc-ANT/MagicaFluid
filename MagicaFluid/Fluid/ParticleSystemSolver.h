#pragma once
#include "pch.h"

#include "PhysicsAnimation.h"
#include "ParticleSystemData.h"

class ParticleSystemSolver : public PhysicsAnimation
{
public:
	ParticleSystemSolver();
	virtual ~ParticleSystemSolver();

	Vector3D worldSize();
	Vector3D gravity();

	const ParticleSystemDataPtr& particleSystemData() const;
	void setParticleSystemData(const ParticleSystemDataPtr& newParticles);

protected:
	void onInitialize() override;

	void onAdvanceTimeStep(double timeIntervalInSeconds) override;

	virtual void accumulateForces(double timeStepInSeconds);

	virtual void onBeginAdvanceTimeStep(double timeStepInSeconds);

	virtual void onEndAdvanceTimeStep(double timeStepInSeconds);

	void resolveCollision();
	void resolveCollision(VectorArray& newPositions,VectorArray& newVelocities);

private:
	bool _systemRun = false;

	double _dragCoefficient = 1e-4;
	double _restitutionCoefficient = 0.0;
	double _wallDamping = -0.5;
	Vector3D _gravity = Vector3D(0.0, -9.8, 0.0);
	Vector3D _wordSize = Vector3D(1, 2, 1);

	ParticleSystemDataPtr _particleSystemData;
	VectorArray _newPositions;
	VectorArray _newVelocities;

	void beginAdvanceTimeStep(double timeStepInSeconds);

	void endAdvanceTimeStep(double timeStepInSeconds);

	void accumulateExternalForces();

	void timeIntegration(double timeIntervalInSeconds);

};