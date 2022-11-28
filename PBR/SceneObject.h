#pragma once
#include "pch.h"
#include "Transform.h"
#include "Ray.h"
#include "CollisionInfo.h"
#include "BoundingBox.h"
#include "Mesh.h"
#include "Bone.h"

class SceneObject {
public:
	std::string name;
	SceneObject* parent;
	std::vector<SceneObject*> children;

	std::map<std::string, BoneInfo> boneInfoMap;
	int32_t boneCounter;

	std::map<std::string, BoneInfo>& GetBoneInfoMap();
	int32_t& GetBoneCount();

	SceneObject(const std::string& _name = "", const Transform& _transform = Transform(), uint32_t _materialIndex = 0);
	~SceneObject();

	void AddChild(SceneObject* child);

	void SetTransform(const Transform& transform);
	const Transform& GetTransform() const;
	
	void SetMesh(const Mesh& mesh);
	const Mesh& GetMesh() const;
	void SetMaterial(int32_t _materialIndex);
	int32_t GetMaterial() const;

	bool TestCollision(const Ray& ray, float tMin, float tMax, CollisionInfo& outCollision) const;

protected:
	Transform transform;
	uint32_t materialIndex;
	Mesh mesh;
};

