#pragma once
#include "pch.h"
#include "Ray.h"
#include "Transform.h"
#include "BoundingBox.h"
#include "CollisionInfo.h"

#define MAX_BONES_PER_VERTEX 4

struct Vertex {
	glm::vec3 p;
	glm::vec3 n;
	glm::vec2 uv;
	int32_t boneIDs[MAX_BONES_PER_VERTEX];
	float boneWeights[MAX_BONES_PER_VERTEX];
	Vertex(const glm::vec3& _p = glm::vec3(0), const glm::vec3& _n = glm::vec3(0), const glm::vec2& _uv = glm::vec2(0));
};

class Mesh {
public:
	std::vector<Vertex> vertices;
	std::vector<int32_t> indices;

	Mesh(const std::vector<glm::vec3>& _positions = std::vector<glm::vec3>(), const std::vector<int32_t>& indeces = std::vector<int32_t>());
	~Mesh();

	bool TestCollision(const Ray& ray, float tMin, float tMax, CollisionInfo& out_collision) const;
	void ApplyTransform(const Transform& transform);
	void SetMaterial(int32_t _materialIndex);
	const BoundingBox& GetWorldBounds() const;

private:
	Transform transform; // save applied transform to revert it
	BoundingBox worldBounds;
	int32_t materialIndex;

	void UpdateWorldBounds();

public:
	static Mesh CreateBox(const glm::vec3 size);
};

