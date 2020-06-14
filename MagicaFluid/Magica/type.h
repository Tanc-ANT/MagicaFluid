#pragma once
#include "pch.h"
#include "glm/glm.hpp"

typedef glm::u32 uint;
typedef glm::vec3 float3;
typedef glm::u32vec3 uint3;
typedef glm::i32vec3 int3;

typedef glm::i64 int64;
typedef glm::u64vec3 Size3;
typedef glm::dvec3 Vector3D;
typedef std::vector<Vector3D> VectorArray;

constexpr double kBoundary = 0.0001;
constexpr double kWaterDensity = 1000.0;
constexpr double kSpeedOfSoundInWater = 1482.0;
constexpr double kPiD = 3.14159265358979323846264338327950288;
constexpr double kEpsilonD = std::numeric_limits<double>::epsilon();
