#pragma once
#include "pch.h"
#include "type.h"

class BoundingBox
{
public:
	Vector3D lowerCorner;

	Vector3D upperCorner;

	BoundingBox();

	BoundingBox(const Vector3D& point1, const Vector3D& point2);

	BoundingBox(const BoundingBox& other);

	double width() const;

	double height() const;

	double depth() const;

	double length(size_t axis);

	bool overlaps(const BoundingBox& other) const;

	bool contains(const Vector3D& point) const;

	
	//bool intersects(const Ray3<T>& ray) const;

	//BoundingBoxRayIntersection3<T> closestIntersection(
	//	const Ray3<T>& ray) const;

	Vector3D midPoint() const;

	double diagonalLength() const;

	double diagonalLengthSquared() const;

	void reset();

	void merge(const Vector3D& point);

	void merge(const BoundingBox& other);

	void expand(double delta);

	Vector3D corner(size_t idx) const;
};