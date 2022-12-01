#pragma once
#include "pch.h"
#include "BoundingBox.h"
#include "CollisionInfo.h"

struct CachedTriangle {
	glm::vec3 v0, v1, v2;
	glm::vec3 edge0, edge1, edge2;
	glm::vec3 normal;
	BoundingBox bounds;
	float area;
	float d;

	bool TestCollision(const Ray& ray, float tMin, float tMax, CollisionInfo& outCollision) const;
};

class MeshBVH {
public:
	const uint32_t maxTriangles = 32;

	std::vector<MeshBVH*> children;
	std::vector<CachedTriangle> triangles;
	BoundingBox bounds;

	MeshBVH(std::vector<CachedTriangle>& _triangles, int32_t axisIndex = 0, int32_t start = 0, int32_t end = -1);
	~MeshBVH();

	bool CheckCollision(const Ray& ray, float tMin, float tMax, CollisionInfo& outCollision) const;
};