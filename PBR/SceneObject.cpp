#include "pch.h"
#include "SceneObject.h"

SceneObject::SceneObject(const std::string& _name, const Transform& transform, uint32_t materialIndex)
	: name(_name), transform(transform), materialIndex(materialIndex), parent(nullptr) {}

SceneObject::~SceneObject() {
	for (int32_t i = 0; i < children.size(); i++) {
		delete children[i];
	}
}

void SceneObject::AddChild(SceneObject* child) {
	child->parent = this;
	children.push_back(child);
}

void SceneObject::SetTransform(const Transform& _transform) {
	transform = _transform;
	mesh.ApplyTransform(transform);
	for (int32_t i = 0; i < children.size(); i++) {
		children[i]->SetTransform(_transform);
	}
}

const Transform& SceneObject::GetTransform() const {
	return transform;
}

void SceneObject::SetMesh(const Mesh& _mesh) {
	mesh = _mesh;
	mesh.SetMaterial(materialIndex);
	mesh.ApplyTransform(transform);
}

const Mesh& SceneObject::GetMesh() const {
	return mesh;
}

void SceneObject::SetMaterial(int32_t _materialIndex) {
	materialIndex = _materialIndex;
	mesh.SetMaterial(materialIndex);
}

int32_t SceneObject::GetMaterial() const {
	return materialIndex;
}

bool SceneObject::TestCollision(const Ray& ray, float tMin, float tMax, CollisionInfo& outCollision) const {
	return mesh.TestCollision(ray, tMin, tMax, outCollision);
}

std::map<std::string, BoneInfo>& SceneObject::GetBoneInfoMap() { 
	return boneInfoMap; 
}

int32_t& SceneObject::GetBoneCount() {
	return boneCounter; 
}