#include "pch.h"
#include "type.h"

struct SphStdKernel
{
	double h;
	double h2;
	double h3;
	double h5;

	inline SphStdKernel()
		: h(0), h2(0), h3(0), h5(0) {}

	inline SphStdKernel(double kernelRadius)
		: h(kernelRadius), h2(h* h), h3(h2* h), h5(h2* h3) {}

	inline SphStdKernel(const SphStdKernel& other)
		: h(other.h), h2(other.h2), h3(other.h3), h5(other.h5) {}

	inline double operator()(double distance) const
	{
		if (distance * distance >= h2) {
			return 0.0;
		}
		else {
			double x = 1.0 - distance * distance / h2;
			return 315.0 / (64.0 * kPiD * h3) * x * x * x;
		}
	}

	inline double firstDerivative(double distance) const
	{
		if (distance >= h) {
			return 0.0;
		}
		else {
			double x = 1.0 - distance * distance / h2;
			return -945.0 / (32.0 * kPiD * h5) * distance * x * x;
		}
	}

	inline Vector3D gradient(
		const Vector3D& point) const
	{
		double dist = std::sqrt(
			point.x * point.x +
			point.y * point.y +
			point.z * point.z);
		if (dist > 0.0) {
			return gradient(dist, point / dist);
		}
		else {
			return Vector3D(0, 0, 0);
		}
	}

	inline Vector3D gradient(
		double distance,
		const Vector3D& directionToCenter) const
	{
		return -firstDerivative(distance) * directionToCenter;
	}

	inline double secondDerivative(double distance) const
	{
		if (distance * distance >= h2) {
			return 0.0;
		}
		else {
			double x = distance * distance / h2;
			return 945.0 / (32.0 * kPiD * h5) * (1 - x) * (3 - 7 * x);
		}
	}
};

struct SphSpikyKernel
{
	double h;

	double h2;

	double h3;

	double h4;

	double h5;

	inline SphSpikyKernel()
		: h(0), h2(0), h3(0), h4(0), h5(0) {}

	inline SphSpikyKernel(double h_)
		: h(h_), h2(h* h), h3(h2* h), h4(h2* h2), h5(h3* h2) {}

	inline SphSpikyKernel(const SphSpikyKernel& other)
		: h(other.h), h2(other.h2), h3(other.h3), h4(other.h4), h5(other.h5) {}

	inline double operator()(double distance) const
	{
		if (distance >= h) {
			return 0.0;
		}
		else {
			double x = 1.0 - distance / h;
			return 15.0 / (kPiD * h3) * x * x * x;
		}
	}

	inline double firstDerivative(double distance) const
	{
		if (distance >= h) {
			return 0.0;
		}
		else {
			double x = 1.0 - distance / h;
			return -45.0 / (kPiD * h4) * x * x;
		}
	}

	inline Vector3D gradient(
		const Vector3D& point) const
	{
		double dist = std::sqrt(
			point.x * point.x +
			point.y * point.y +
			point.z * point.z);
		if (dist > 0.0) {
			return gradient(dist, point / dist);
		}
		else {
			return Vector3D(0, 0, 0);
		}
	}

	inline Vector3D gradient(
		double distance,
		const Vector3D& directionToCenter) const
	{
		return -firstDerivative(distance) * directionToCenter;
	}

	inline double secondDerivative(double distance) const
	{
		if (distance >= h) {
			return 0.0;
		}
		else {
			double x = distance / h;
			return -90.0 / (kPiD * h4) * (1 - x) * (1 - 2 * x);
			//double x = 1 - distance / h;
			//return 90.0 / (kPiD * h5) * x;
		}
	}
};

struct SphViscosityKernel
{
	double h;

	double h2;

	double h3;

	double h4;

	double h5;

	inline SphViscosityKernel()
		: h(0), h2(0), h3(0), h4(0), h5(0) {}

	inline SphViscosityKernel(double h_)
		: h(h_), h2(h* h), h3(h2* h), h4(h2* h2), h5(h3* h2) {}

	inline SphViscosityKernel(const SphSpikyKernel& other)
		: h(other.h), h2(other.h2), h3(other.h3), h4(other.h4), h5(other.h5) {}

	inline double operator()(double distance) const
	{
		if (distance >= h) {
			return 0.0;
		}
		else {
			double x = distance / h;
			double y = h / distance;
			double item1 = (-1 / 2 * (x * x * x));
			double item2 = x * x;
			double itme3 = 1 / 2 * y;
			return 15.0 / (2 * kPiD * h3) * (item1 + item2 + itme3 - 1);
		}
	}

	inline double firstDerivative(double distance) const
	{
		if (distance >= h) {
			return 0.0;
		}
		else {
			double x = distance / h;
			double y = h / distance;
			double item1 = -(y * y * y) / 2;
			double item2 = -3 * x / 2;
			return 15.0 / (2 * kPiD * h5) * distance * (item1 + item2 + 2);
		}
	}

	inline Vector3D gradient(
		const Vector3D& point) const
	{
		double dist = glm::length(point);

		if (dist > 0.0) {
			return gradient(dist, point / dist);
		}
		else {
			return Vector3D(0, 0, 0);
		}
	}

	inline Vector3D gradient(
		double distance,
		const Vector3D& directionToCenter) const
	{
		return -firstDerivative(distance) * directionToCenter;
	}

	inline double secondDerivative(double distance) const
	{
		if (distance >= h) {
			return 0.0;
		}
		else {
			double x = 1.0 - distance / h;
			return 45.0 / (kPiD * h5) * x;
		}
	}
};