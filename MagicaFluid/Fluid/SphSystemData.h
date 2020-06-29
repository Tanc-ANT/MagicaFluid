#pragma once
#include "pch.h"
#include "type.h"

#include "ParticleSystemData.h"
#include "Kernel.h"

class SphSystemData : public ParticleSystemData
{
public:
	SphSystemData();
	explicit SphSystemData(size_t numberOfParticles);
	virtual ~SphSystemData();

	virtual void resize(size_t newNumberOfParticles) override;

	void setRadius(double newRadius) override;
	void setMass(double newMass) override;
	

	std::vector<double>& densities();
	std::vector<double>& pressures();

	double targetDensity() const;
	void setTargetDensity(double targetDensity);

	double targetSpacing() const;
	void setTargetSpacing(double spacing);

	double relativeKernelRadius() const;
	void setRelativeKernelRadius(double relativeRadius);

	double kernelRadius() const;

	void updateDensities();

	double sumOfKernelNearby(const Vector3D& position);

	double interpolate(const Vector3D& origin, const std::vector<double>& values);

	Vector3D interpolate(const Vector3D& origin, const VectorArray& values);

	Vector3D gradientAt(size_t i, const std::vector<double>& values);

	double laplacianAt(size_t i, const std::vector<double>& values);

	Vector3D laplacianAt(size_t i, const VectorArray& values);

	void buildNeighborSearcher();
	void buildNeighborLists();

private:
	double _targetDensity = kWaterDensity;
	double _targetSpacing = 0.1;
	double _kernelRadiusOverTargetSpacing = 1.8;
	double _kernelRadius;

	std::vector<double> _pressure;
	std::vector<double> _density;

	void computeMass();
};

typedef std::shared_ptr<SphSystemData> SphSystemDataPtr;