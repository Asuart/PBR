#include "pch.h"
#include "SpaceTreeNode.h"

SpaceTreeNode::SpaceTreeNode(std::vector<SceneObject*> _objects, int32_t axisIndex)
	: leaf(false)
{
	BoundingBox box;
	for (int32_t i = 0; i < _objects.size(); i++) {
		BoundingBox objectBox = _objects[i]->GetMesh().GetWorldBounds();
		box = SurroundingBox(box, objectBox);
	}
	bounds = box;

	if (_objects.size() < maxObject) {
		leaf = true;
		for (uint32_t i = 0; i < _objects.size(); i++) {
			objects.push_back(_objects[i]);
		}
		return;
	}

	std::sort(_objects.begin(), _objects.end(),
		[axisIndex](SceneObject* const& a, SceneObject* const& b) { 
			BoundingBox box1 = a->GetMesh().GetWorldBounds();
			BoundingBox box2 = b->GetMesh().GetWorldBounds();
			return box1.pMin[axisIndex] < box2.pMin[axisIndex]; 
		});

	std::vector<SceneObject*> left, right;
	for (uint32_t i = 0; i < _objects.size() / 2; i++) {
		left.push_back(_objects[i]);
	}
	for (uint32_t i = _objects.size() / 2; i < _objects.size(); i++) {
		right.push_back(_objects[i]);
	}

	children.push_back(new SpaceTreeNode(left, (axisIndex + 1) % 3));
	children.push_back(new SpaceTreeNode(right, (axisIndex + 1) % 3));
}

SpaceTreeNode::~SpaceTreeNode() {
	for (uint32_t i = 0; i < children.size(); i++) {
		delete children[i];
	}
}

CollisionInfo SpaceTreeNode::CheckCollision(Ray& ray, float tMin, float tMax) const {
	if (!bounds.TestHit(ray, tMin, tMax)) 
		return CollisionInfo();
	if (leaf) {
		CollisionInfo outCollision, col;
		for (uint32_t i = 0; i < objects.size(); i++) {
			if (objects[i]->TestCollision(ray, tMin, tMax, col)) {
				tMax = col.distance;
				outCollision = col;
			}
		}
		return outCollision;
	}
	CollisionInfo col;
	for (uint32_t i = 0; i < children.size(); i++) {
		CollisionInfo newCol = children[i]->CheckCollision(ray, tMin, tMax);
		if (newCol.distance < col.distance)
			col = newCol;
	}
	return col;
}



