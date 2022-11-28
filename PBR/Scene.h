#pragma once
#include "pch.h"
#include "SceneObject.h"
#include "Sphere.h"
#include "Camera.h"
#include "SpaceTreeNode.h"
#include "Textures.h"
#include "Material.h"

class Scene {
public:
	std::vector<std::shared_ptr<Texture>> textures;
	std::vector<Material> materials;
	std::vector<Sphere> spheres;
	std::vector<Camera> cameras;
	SceneObject* rootObject;
	Camera* mainCamera;

	Scene();
	~Scene();

	void Update();

	void CreateBoxScene();
	void CreateTestScene();
	void CreateRandomScene();
	void CreateCornellBoxScene();

	bool CheckCollision(Ray ray, float tMin, float tMax, CollisionInfo& outCollision) const;

	const std::vector<SceneObject*>& GetObjectWithMeshes() const;

	std::vector<SceneObject*> objectsWithMeshes;
private:
	SpaceTreeNode* spaceTree;

	void UpdateObjectsWithMeshes();
	void FillObjectsWithMeshesList(SceneObject* obj);

	friend class SceneLoader;
};

