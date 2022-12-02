#include "pch.h"
#include "SceneLoader.h"

Scene* SceneLoader::currentScene = nullptr;
SceneObject* SceneLoader::currentObject = nullptr;

Scene* SceneLoader::LoadScene(const std::string& filePath) {
	std::cout << "Loading scene from file: " << filePath << "\n";

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << "\n";
		return nullptr;
	}

	currentScene = new Scene();
	currentObject = new SceneObject(scene->mRootNode->mName.C_Str());

	ProcessNode(scene->mRootNode, scene);

	currentScene->rootObject = currentObject;
	currentScene->UpdateObjectsWithMeshes();
	Scene* s = currentScene;
	currentScene = nullptr;
	return s;
}

SceneObject* SceneLoader::LoadObject(const std::string& filePath, Scene* _scene) {
	currentScene = _scene;
	std::cout << "Loading scene from file: " << filePath << "\n";

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << "\n";
		return nullptr;
	}

	currentObject = new SceneObject(scene->mRootNode->mName.C_Str());

	ProcessNode(scene->mRootNode, scene);

	SceneObject* s = currentObject;
	currentObject = nullptr;
	currentScene = nullptr;
	return s;
}

void SceneLoader::ProcessNode(aiNode* node, const aiScene* scene) {
	if (node->mNumMeshes > 1) {
		std::cout << "Scene Loader: Only one mesh allowed per node.\n";
	}

	if (node->mNumMeshes == 1) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[0]];
		ProcessMesh(mesh, scene);
	}

	for (int32_t i = 0; i < node->mNumChildren; i++) {
		currentObject->AddChild(new SceneObject(node->mChildren[i]->mName.C_Str()));
		currentObject = currentObject->children[i];
		ProcessNode(node->mChildren[i], scene);
		currentObject = currentObject->parent;
	}
}

void SceneLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene) {
	std::cout << "  Mesh: " << mesh->mName.C_Str() << " (vertices: " << mesh->mNumVertices << ")\n";

	Mesh objectMesh;
	objectMesh.vertices.resize(mesh->mNumVertices);
	for (int32_t i = 0; i < mesh->mNumVertices; i++) {
		objectMesh.vertices[i].p.x = mesh->mVertices[i].x;
		objectMesh.vertices[i].p.y = mesh->mVertices[i].y;
		objectMesh.vertices[i].p.z = mesh->mVertices[i].z;
		objectMesh.vertices[i].n.x = mesh->mNormals[i].x;
		objectMesh.vertices[i].n.y = mesh->mNormals[i].y;
		objectMesh.vertices[i].n.z = mesh->mNormals[i].z;
	}

	for (int32_t i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (int32_t j = 0; j < face.mNumIndices; j++)
			objectMesh.indices.push_back(face.mIndices[j]);
	}

	if (mesh->mMaterialIndex >= 0) {
		std::cout << "Material name: " << scene->mMaterials[mesh->mMaterialIndex]->GetName().C_Str() << "\n";
		aiColor4D color, colorEmissive, emissionIntensity;
		float emissionInt, roughness;
		aiGetMaterialColor(scene->mMaterials[mesh->mMaterialIndex], AI_MATKEY_COLOR_DIFFUSE, &color);
		aiGetMaterialFloat(scene->mMaterials[mesh->mMaterialIndex], AI_MATKEY_EMISSIVE_INTENSITY, &emissionInt);
		aiGetMaterialColor(scene->mMaterials[mesh->mMaterialIndex], AI_MATKEY_EMISSIVE_INTENSITY, &emissionIntensity);
		aiGetMaterialColor(scene->mMaterials[mesh->mMaterialIndex], AI_MATKEY_COLOR_EMISSIVE, &colorEmissive);
		aiGetMaterialFloat(scene->mMaterials[mesh->mMaterialIndex], AI_MATKEY_ROUGHNESS_FACTOR, &roughness);

		Material mat;
		mat.albedo = glm::vec3(color.r, color.g, color.b);
		mat.roughness = 1.0;
		mat.lightColor = glm::vec3(colorEmissive.r, colorEmissive.g, colorEmissive.b);

		if (std::string(scene->mMaterials[mesh->mMaterialIndex]->GetName().C_Str()) == std::string("Glass")) {
			mat.transparency = 1.0;
			mat.refraction = 1.7;
		}


		std::cout << "	albedo: " << mat.albedo.x << " " << mat.albedo.y << " " << mat.albedo.z << "\n";
		std::cout << "	emissionInt: " << emissionIntensity.r << " " << emissionIntensity.g << " " << emissionIntensity.b << " " << emissionIntensity.a << " (" << emissionInt << ")\n";
		std::cout << "	colorEmissive: " << colorEmissive.r << " " << colorEmissive.g << " " << colorEmissive.b << " " << colorEmissive.a << "\n";
		std::cout << "	roughness: " << roughness << "\n";

		objectMesh.SetMaterial(currentScene->materials.size());
		currentObject->SetMaterial(currentScene->materials.size());
		currentScene->materials.push_back(mat);
	}

	ExtractBoneWeightForVertices(objectMesh.vertices, mesh, scene);

	currentObject->SetMesh(objectMesh);
}

void SceneLoader::SetVertexBoneData(Vertex& vertex, int boneID, float weight) {
	for (int i = 0; i < MAX_BONES_PER_VERTEX; i++) {
		if (vertex.boneIDs[i] < 0) {
			vertex.boneWeights[i] = weight;
			vertex.boneIDs[i] = boneID;
			break;
		}
	}
}

void SceneLoader::ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene) {
	for (int32_t boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++) {
		int32_t boneID = -1;
		std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
		if (currentObject->boneInfoMap.find(boneName) == currentObject->boneInfoMap.end()) {
			BoneInfo newBoneInfo;
			newBoneInfo.id = currentObject->boneCounter;
			newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
			currentObject->boneInfoMap[boneName] = newBoneInfo;
			boneID = currentObject->boneCounter;
			currentObject->boneCounter++;
		}
		else {
			boneID = currentObject->boneInfoMap[boneName].id;
		}
		assert(boneID != -1);
		auto weights = mesh->mBones[boneIndex]->mWeights;
		int32_t numWeights = mesh->mBones[boneIndex]->mNumWeights;
		for (int32_t weightIndex = 0; weightIndex < numWeights; weightIndex++) {
			int32_t vertexId = weights[weightIndex].mVertexId;
			float weight = weights[weightIndex].mWeight;
			assert(vertexId <= vertices.size());
			SetVertexBoneData(vertices[vertexId], boneID, weight);
		}
	}
}