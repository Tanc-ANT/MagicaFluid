#pragma once
#include "pch.h"
#include "type.h"

typedef std::function <void(size_t, const Vector3D&)> ForEachNearbyPointFunc;

class NeighborSearcher
{
public:

	NeighborSearcher(const Size3& resolution, double gridSpacing);
	NeighborSearcher(
		size_t resolutionX,
		size_t resolutionY,
		size_t resolutionZ,
		double gridSpacing);
	virtual ~NeighborSearcher();

	virtual void build(const VectorArray& points);

	virtual void forEachNearbyPoint(const Vector3D& origin, double radius, const ForEachNearbyPointFunc& callback) const;

	Size3 getBucketIndex(const Vector3D& position) const;

	size_t getHashKeyFromBucketIndex(const Size3& bucketIndex) const;

private:
	double _gridSpacing = 8.0;
	Size3 _resolution = Size3(1, 1, 1);
	VectorArray _points;
	std::vector<std::vector<size_t>> _buckets;

	size_t getHashKeyFromPosition(const Vector3D& position) const;

	void getNearbyKeys(const Vector3D& position, size_t* bucketIndices) const;
};

typedef std::shared_ptr<NeighborSearcher> NeighborSearcherPtr;