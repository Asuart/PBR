#pragma once
#include "pch.h"
#include "BoundingBox.h"
#include "SceneObject.h"

class SpaceTreeNode {
public:
	const uint32_t maxObject = 16;

	bool leaf;
	std::vector<SpaceTreeNode*> children;
	std::vector<SceneObject*> objects;
	BoundingBox bounds;

	SpaceTreeNode(std::vector<SceneObject*> _objects, int32_t axisIndex = 0, float time0 = 0.0, float time1 = 1.0);
	~SpaceTreeNode();

	CollisionInfo CheckCollision(Ray& ray, float tMin, float tMax) const;
};

