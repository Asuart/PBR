#pragma once
#include "pch.h"
#include "BoundingBox.h"
#include "SceneObject.h"

class SceneObject;

class SpaceTreeNode {
public:
	const uint32_t maxObject = 3;

	bool leaf;
	std::vector<SpaceTreeNode*> children;
	std::vector<SceneObject*> objects;
	BoundingBox bounds;

	SpaceTreeNode(std::vector<SceneObject*> _objects, int32_t axisIndex = 0);
	~SpaceTreeNode();

	CollisionInfo CheckCollision(Ray& ray, float tMin, float tMax) const;
};

