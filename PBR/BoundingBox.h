#pragma once
#include "pch.h"
#include "Ray.h"

struct BoundingBox {
	glm::vec3 pMin;
	glm::vec3 pMax;

	BoundingBox(const glm::vec3& _pMin = glm::vec3(0), const glm::vec3& _pMax = glm::vec3(0));
	~BoundingBox();

	bool IsIntersected(const Ray& ray, float* out_hit0 = nullptr, float* out_hit1 = nullptr) const;
	bool TestHit(const Ray& tay, float tMin, float tMax) const;

	glm::vec3 Diagonal() const;
	float SurfaceArea() const;
	float Volume() const;
};

BoundingBox SurroundingBox(const BoundingBox& box0, const BoundingBox& box1);