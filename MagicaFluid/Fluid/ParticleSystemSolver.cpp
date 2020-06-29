#include "ParticleSystemSolver.h"

ParticleSystemSolver::ParticleSystemSolver()
{
	_particleSystemData = std::make_shared<ParticleSystemData>();
}

ParticleSystemSolver::~ParticleSystemSolver()
{

}

Vector3D ParticleSystemSolver::worldSize()
{
	return _wordSize;
}

Vector3D ParticleSystemSolver::gravity()
{
	return _gravity;
}

const ParticleSystemDataPtr& ParticleSystemSolver::particleSystemData() const
{
	return _particleSystemData;
}

void ParticleSystemSolver::setParticleSystemData(const ParticleSystemDataPtr& newParticles)
{
	_particleSystemData = newParticles;
}

void ParticleSystemSolver::onInitialize()
{
	Vector3D pos;
	Vector3D vel;
	vel.x = 0.0f;
	vel.y = 0.0f;
	vel.z = 0.0f;
	const double radius = _particleSystemData->radius();

	VectorArray postions;
	VectorArray velositys;

	for (pos.x = _wordSize.x * 0.0f; pos.x < _wordSize.x * 0.6f; pos.x += 0.8*radius)
	{
		for (pos.y = _wordSize.y * 0.0f; pos.y < _wordSize.y * 0.9f; pos.y += 0.8*radius)
		{
			for (pos.z = _wordSize.z * 0.0f; pos.z < _wordSize.z * 0.6f; pos.z += 0.8*radius)
			{
				postions.push_back(pos);
				velositys.push_back(vel);
			}
		}
	}

	_particleSystemData->addParticles(postions, velositys);

	std::cout << "Init Particle: " << _particleSystemData->numberOfParticles() << std::endl;
}


void ParticleSystemSolver::onAdvanceTimeStep(double timeIntervalInSeconds)
{
	beginAdvanceTimeStep(timeIntervalInSeconds);
	accumulateForces(timeIntervalInSeconds);
	timeIntegration(timeIntervalInSeconds);
	resolveCollision();
	endAdvanceTimeStep(timeIntervalInSeconds);
}

void ParticleSystemSolver::accumulateForces(double timeIntervalInSeconds)
{
	accumulateExternalForces();
}

void ParticleSystemSolver::onBeginAdvanceTimeStep(double timeStepInSeconds)
{
	// Do nothing
}

void ParticleSystemSolver::onEndAdvanceTimeStep(double timeStepInSeconds)
{
	// Do nothing
}

void ParticleSystemSolver::accumulateExternalForces()
{
	const size_t n = _particleSystemData->numberOfParticles();
	auto& forces = _particleSystemData->forces();
	auto& velocities = _particleSystemData->velocities();
	auto& positions = _particleSystemData->positions();
	const auto mass = _particleSystemData->mass();

	for (int i = 0; i < n; ++i)
	{
		// Gravity
		Vector3D force = mass * _gravity;

		// Wind is not implement yet

		// caculate force
		forces[i] += force;
	}
}

void ParticleSystemSolver::timeIntegration(double timeIntervalInSeconds)
{
	size_t n = _particleSystemData->numberOfParticles();
	auto& forces = _particleSystemData->forces();
	auto& velocities = _particleSystemData->velocities();
	auto& positions = _particleSystemData->positions();
	auto mass = _particleSystemData->mass();

	for (int i = 0; i < n; ++i)
	{
		// Integrate velocity first
		Vector3D& newVelocity = _newVelocities[i];
		newVelocity = velocities[i] + timeIntervalInSeconds * forces[i] / mass;

		// Integrate position
		Vector3D& newPosition = _newPositions[i];
		newPosition = positions[i] + timeIntervalInSeconds * newVelocity;
	}
}

void ParticleSystemSolver::resolveCollision()
{
	resolveCollision(_newPositions, _newVelocities);
}

void ParticleSystemSolver::resolveCollision(VectorArray& newPositions, VectorArray& newVelocities)
{
	for (int i = 0; i < _particleSystemData->numberOfParticles(); ++i)
	{
		if (newPositions[i].x >= _wordSize.x - kBoundary)
		{
			newVelocities[i].x = newVelocities[i].x * _wallDamping;
			newPositions[i].x = _wordSize.x - kBoundary;
		}

		if (newPositions[i].x < 0.0f)
		{
			newVelocities[i].x = newVelocities[i].x * _wallDamping;
			newPositions[i].x = 0.0f;
		}

		if (newPositions[i].y >= _wordSize.y - kBoundary)
		{
			newVelocities[i].y = newVelocities[i].y * _wallDamping;
			newPositions[i].y = _wordSize.y - kBoundary;
		}

		if (newPositions[i].y < 0.0f)
		{
			newVelocities[i].y = newVelocities[i].y * _wallDamping;
			newPositions[i].y = 0.0f;
		}

		if (newPositions[i].z >= _wordSize.z - kBoundary)
		{
			newVelocities[i].z = newVelocities[i].z * _wallDamping;
			newPositions[i].z = _wordSize.z - kBoundary;
		}

		if (newPositions[i].z < 0.0f)
		{
			newVelocities[i].z = newVelocities[i].z * _wallDamping;
			_newPositions[i].z = 0.0f;
		}
	}
}

void ParticleSystemSolver::beginAdvanceTimeStep(double timeStepInSeconds)
{
	// Clear forces
	auto& forces = _particleSystemData->forces();
	forces.clear();
	forces.resize(_particleSystemData->numberOfParticles());

	// Allocate buffers
	size_t n = _particleSystemData->numberOfParticles();
	_newPositions.resize(n);
	_newVelocities.resize(n);

	onBeginAdvanceTimeStep(timeStepInSeconds);
}

void ParticleSystemSolver::endAdvanceTimeStep(double timeStepInSeconds) {
	// Update data
	size_t n = _particleSystemData->numberOfParticles();
	auto& velocities = _particleSystemData->velocities();
	auto& positions = _particleSystemData->positions();

	for (int i = 0; i < n; ++i)
	{
		positions[i] = _newPositions[i];
		velocities[i] = _newVelocities[i];
	}

	onEndAdvanceTimeStep(timeStepInSeconds);
}

