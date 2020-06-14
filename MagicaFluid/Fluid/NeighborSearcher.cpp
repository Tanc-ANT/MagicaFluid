#include "NeighborSearcher.h"

NeighborSearcher::NeighborSearcher(
	const Size3& resolution, 
	double gridSpacing)	:
	NeighborSearcher(
		resolution.x,
		resolution.y,
		resolution.z,
		gridSpacing)
{
}

NeighborSearcher::NeighborSearcher(
	size_t resolutionX,
	size_t resolutionY,
	size_t resolutionZ,
	double gridSpacing) :
	_gridSpacing(gridSpacing) {
	_resolution.x = std::max(static_cast<int64>(resolutionX), static_cast <int64>(0));
	_resolution.y = std::max(static_cast<int64>(resolutionY), static_cast <int64>(0));
	_resolution.z = std::max(static_cast<int64>(resolutionZ), static_cast <int64>(0));
}

NeighborSearcher::~NeighborSearcher()
{
}

void NeighborSearcher::build(const VectorArray& points)
{
	_buckets.clear();
	_points.clear();

	// Allocate memory chuncks
	_buckets.resize(_resolution.x * _resolution.y * _resolution.z);
	_points.resize(points.size());

	if (points.size() == 0) {
		return;
	}

	// Put points into buckets
	for (size_t i = 0; i < points.size(); ++i) {
		_points[i] = points[i];
		size_t key = getHashKeyFromPosition(points[i]);
		_buckets[key].push_back(i);
	}
}

void NeighborSearcher::forEachNearbyPoint(
	const Vector3D& origin,
	double radius,
	const ForEachNearbyPointFunc& callback) const
{
	if (_buckets.empty()) {
		return;
	}

	size_t nearbyKeys[8];
	getNearbyKeys(origin, nearbyKeys);

	const double queryRadiusSquared = radius * radius;

	for (int i = 0; i < 8; i++) {
		const auto& bucket = _buckets[nearbyKeys[i]];
		size_t numberOfPointsInBucket = bucket.size();

		for (size_t j = 0; j < numberOfPointsInBucket; ++j) {
			size_t pointIndex = bucket[j];
			Vector3D point = _points[pointIndex] - origin;
			double rSquared = point.x * point.x + point.y * point.y + point.z * point.z;
			if (rSquared <= queryRadiusSquared) {
				callback(pointIndex, _points[pointIndex]);
			}
		}
	}
}

Size3 NeighborSearcher::getBucketIndex(const Vector3D& position) const 
{
	Size3 bucketIndex;
	bucketIndex.x = static_cast<int64>(
		std::floor(position.x / _gridSpacing));
	bucketIndex.y = static_cast<int64>(
		std::floor(position.y / _gridSpacing));
	bucketIndex.z = static_cast<int64>(
		std::floor(position.z / _gridSpacing));
	return bucketIndex;
}

size_t NeighborSearcher::getHashKeyFromPosition(
	const Vector3D& position) const 
{
	Size3 bucketIndex = getBucketIndex(position);
	return getHashKeyFromBucketIndex(bucketIndex);
}

size_t NeighborSearcher::getHashKeyFromBucketIndex(
	const Size3& bucketIndex) const
{
	Size3 wrappedIndex = bucketIndex;
	wrappedIndex.x = bucketIndex.x % _resolution.x;
	wrappedIndex.y = bucketIndex.y % _resolution.y;
	wrappedIndex.z = bucketIndex.z % _resolution.z;
	if (wrappedIndex.x < 0) {
		wrappedIndex.x += _resolution.x;
	}
	if (wrappedIndex.y < 0) {
		wrappedIndex.y += _resolution.y;
	}
	if (wrappedIndex.z < 0) {
		wrappedIndex.z += _resolution.z;
	}
	return static_cast<size_t>(
		(wrappedIndex.z * _resolution.y + wrappedIndex.y) * _resolution.x
		+ wrappedIndex.x);
}


void NeighborSearcher::getNearbyKeys(
	const Vector3D& position,
	size_t* nearbyKeys) const 
{
	Size3 originIndex = getBucketIndex(position), nearbyBucketIndices[8];

	for (int i = 0; i < 8; i++) {
		nearbyBucketIndices[i] = originIndex;
	}

	if ((originIndex.x + 0.5f) * _gridSpacing <= position.x) {
		nearbyBucketIndices[4].x += 1;
		nearbyBucketIndices[5].x += 1;
		nearbyBucketIndices[6].x += 1;
		nearbyBucketIndices[7].x += 1;
	}
	else {
		nearbyBucketIndices[4].x -= 1;
		nearbyBucketIndices[5].x -= 1;
		nearbyBucketIndices[6].x -= 1;
		nearbyBucketIndices[7].x -= 1;
	}

	if ((originIndex.y + 0.5f) * _gridSpacing <= position.y) {
		nearbyBucketIndices[2].y += 1;
		nearbyBucketIndices[3].y += 1;
		nearbyBucketIndices[6].y += 1;
		nearbyBucketIndices[7].y += 1;
	}
	else {
		nearbyBucketIndices[2].y -= 1;
		nearbyBucketIndices[3].y -= 1;
		nearbyBucketIndices[6].y -= 1;
		nearbyBucketIndices[7].y -= 1;
	}

	if ((originIndex.z + 0.5f) * _gridSpacing <= position.z) {
		nearbyBucketIndices[1].z += 1;
		nearbyBucketIndices[3].z += 1;
		nearbyBucketIndices[5].z += 1;
		nearbyBucketIndices[7].z += 1;
	}
	else {
		nearbyBucketIndices[1].z -= 1;
		nearbyBucketIndices[3].z -= 1;
		nearbyBucketIndices[5].z -= 1;
		nearbyBucketIndices[7].z -= 1;
	}

	for (int i = 0; i < 8; i++) {
		nearbyKeys[i] = getHashKeyFromBucketIndex(nearbyBucketIndices[i]);
	}
}