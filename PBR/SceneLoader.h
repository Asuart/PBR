#pragma once
#include "pch.h"
#include "Mesh.h"
#include "Scene.h"
#include "SceneObject.h"

class Scene;

class SceneLoader {
public:
	static Scene* LoadScene(const std::string& filePath);
	static SceneObject* LoadObject(const std::string& filePath);
private:
	static Scene* currentScene;
	static SceneObject* currentObject;
	static void ProcessNode(aiNode* node, const aiScene* scene);
	static void ProcessMesh(aiMesh* mesh, const aiScene* scene);
	static void SetVertexBoneData(Vertex& vertex, int boneID, float weight);
	static void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);
};