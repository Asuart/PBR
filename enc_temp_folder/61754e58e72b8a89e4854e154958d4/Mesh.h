#pragma once
#include "pch.h"
#include "Ray.h"
#include "Transform.h"
#include "BoundingBox.h"
#include "CollisionInfo.h"
#include "Random.h"

#define MAX_BONES_PER_VERTEX 4

class MeshBVH;

struct Vertex {
	glm::vec3 p;
	glm::vec3 n;
	glm::vec2 uv;
	int32_t boneIDs[MAX_BONES_PER_VERTEX];
	float boneWeights[MAX_BONES_PER_VERTEX];
	Vertex(const glm::vec3& _p = glm::vec3(0), const glm::vec3& _n = glm::vec3(0), const glm::vec2& _uv = glm::vec2(0));
};

struct CachedTriangle {
	glm::vec3 v0, v1, v2;
	glm::vec3 normal;
	glm::vec3 edge0, edge1, edge2;
	float area;
	float d;

	bool TestCollision(const Ray& ray, float tMin, float tMax, CollisionInfo& outCollision) const;
	BoundingBox GetBounds() const;
};

class MeshBVH {
public:
	const uint32_t maxTriangles = 32;

	bool leaf;
	std::vector<MeshBVH*> children;
	std::vector<CachedTriangle> triangles;
	BoundingBox bounds;

	MeshBVH(std::vector<CachedTriangle> _triangles, int32_t axisIndex = 0);
	~MeshBVH();

	bool CheckCollision(const Ray& ray, float tMin, float tMax, CollisionInfo& outCollision) const;
};

class Mesh {
public:
	std::vector<Vertex> vertices;
	std::vector<int32_t> indices;
	std::vector<CachedTriangle> triangles;
	float area;

	Mesh(const std::vector<glm::vec3>& _positions = std::vector<glm::vec3>(), const std::vector<int32_t>& indeces = std::vector<int32_t>());
	~Mesh();

	bool TestCollision(const Ray& ray, float tMin, float tMax, CollisionInfo& out_collision) const;
	void ApplyTransform(const Transform& transform);
	void SetMaterial(int32_t _materialIndex);
	const BoundingBox& GetWorldBounds() const;
	
	glm::vec3 RandomInnerPoint() const;
	void UpdateCachedTriangles();

private:
	Transform transform; // save applied transform to revert it
	BoundingBox worldBounds;
	int32_t materialIndex;

	std::shared_ptr<MeshBVH> bvhRoot;

	void UpdateWorldBounds();
	float Area();

public:
	static Mesh CreateBox(const glm::vec3 size);
};

