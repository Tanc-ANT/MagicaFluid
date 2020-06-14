#include "BoundingBox.h"

BoundingBox::BoundingBox()
{
	reset();
}

BoundingBox::BoundingBox(
	const Vector3D& point1, const Vector3D& point2)
{
	lowerCorner.x = std::min(point1.x, point2.x);
	lowerCorner.y = std::min(point1.y, point2.y);
	lowerCorner.z = std::min(point1.z, point2.z);
	upperCorner.x = std::max(point1.x, point2.x);
	upperCorner.y = std::max(point1.y, point2.y);
	upperCorner.z = std::max(point1.z, point2.z);
}

BoundingBox::BoundingBox(const BoundingBox& other) :
	lowerCorner(other.lowerCorner),
	upperCorner(other.upperCorner) 
{
}

double BoundingBox::width() const
{
	return upperCorner.x - lowerCorner.x;
}

double BoundingBox::height() const
{
	return upperCorner.y - lowerCorner.y;
}

double BoundingBox::depth() const
{
	return upperCorner.z - lowerCorner.z;
}

double BoundingBox::length(size_t axis) {
	return upperCorner[axis] - lowerCorner[axis];
}

bool BoundingBox::overlaps(const BoundingBox& other) const
{
	if (upperCorner.x < other.lowerCorner.x
		|| lowerCorner.x > other.upperCorner.x) {
		return false;
	}

	if (upperCorner.y < other.lowerCorner.y
		|| lowerCorner.y > other.upperCorner.y) {
		return false;
	}

	if (upperCorner.z < other.lowerCorner.z
		|| lowerCorner.z > other.upperCorner.z) {
		return false;
	}

	return true;
}

bool BoundingBox::contains(const Vector3D& point) const
{
	if (upperCorner.x < point.x || lowerCorner.x > point.x) {
		return false;
	}

	if (upperCorner.y < point.y || lowerCorner.y > point.y) {
		return false;
	}

	if (upperCorner.z < point.z || lowerCorner.z > point.z) {
		return false;
	}

	return true;
}

Vector3D BoundingBox::midPoint() const
{
	return (upperCorner + lowerCorner) / static_cast<double>(2);
}

double BoundingBox::diagonalLength() const
{
	Vector3D distance = upperCorner - lowerCorner;;
	double dist = std::sqrt(
		distance.x * distance.x +
		distance.y * distance.y +
		distance.z * distance.z);
	return dist;
}

double BoundingBox::diagonalLengthSquared() const 
{
	Vector3D distance = upperCorner - lowerCorner;;
	double distSquared = distance.x * distance.x +
		distance.y * distance.y +
		distance.z * distance.z;
	return distSquared;
}

void BoundingBox::reset()
{
	lowerCorner.x = std::numeric_limits<double>::max();
	lowerCorner.y = std::numeric_limits<double>::max();
	lowerCorner.z = std::numeric_limits<double>::max();
	upperCorner.x = -std::numeric_limits<double>::max();
	upperCorner.y = -std::numeric_limits<double>::max();
	upperCorner.z = -std::numeric_limits<double>::max();
}

void BoundingBox::merge(const Vector3D& point)
{
	lowerCorner.x = std::min(lowerCorner.x, point.x);
	lowerCorner.y = std::min(lowerCorner.y, point.y);
	lowerCorner.z = std::min(lowerCorner.z, point.z);
	upperCorner.x = std::max(upperCorner.x, point.x);
	upperCorner.y = std::max(upperCorner.y, point.y);
	upperCorner.z = std::max(upperCorner.z, point.z);
}

void BoundingBox::merge(const BoundingBox& other)
{
	lowerCorner.x = std::min(lowerCorner.x, other.lowerCorner.x);
	lowerCorner.y = std::min(lowerCorner.y, other.lowerCorner.y);
	lowerCorner.z = std::min(lowerCorner.z, other.lowerCorner.z);
	upperCorner.x = std::max(upperCorner.x, other.upperCorner.x);
	upperCorner.y = std::max(upperCorner.y, other.upperCorner.y);
	upperCorner.z = std::max(upperCorner.z, other.upperCorner.z);
}

void BoundingBox::expand(double delta)
{
	lowerCorner -= delta;
	upperCorner += delta;
}

Vector3D BoundingBox::corner(size_t idx) const
{
	Vector3D result;
	if (idx & 1) {
		result.x = upperCorner.x;
	}
	else {
		result.x = lowerCorner.x;
	}
	if (idx & 2) {
		result.y = upperCorner.y;
	}
	else {
		result.y = lowerCorner.y;
	}
	if (idx & 4) {
		result.z = upperCorner.z;
	}
	else {
		result.z = lowerCorner.z;
	}
	return result;
}