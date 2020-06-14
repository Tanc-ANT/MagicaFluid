#include "pch.h"
#include "type.h"
#include "BoundingBox.h"

class PointGenerator3 {
public:
	PointGenerator3();

	virtual ~PointGenerator3();

	void generate(
		const BoundingBox& boundingBox,
		double spacing,
		VectorArray& points) const;

	virtual void forEachPoint(
		const BoundingBox& boundingBox,
		double spacing,
		const std::function<bool(const Vector3D&)>& callback) const = 0;
};

typedef std::shared_ptr<PointGenerator3> PointGenerator3Ptr;


class BccLatticePointGenerator final : public PointGenerator3 {
public:
	void forEachPoint(
		const BoundingBox& boundingBox,
		double spacing,
		const std::function<bool(const Vector3D&)>& callback) const override;
};
typedef std::shared_ptr<BccLatticePointGenerator> BccLatticePointGeneratorPtr;