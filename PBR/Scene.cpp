#include "pch.h"
#include "Scene.h"

Scene::Scene() : spaceTree(nullptr), rootObject(nullptr), skyColor(glm::vec3(0)) {
	Material material_ground;
	material_ground.albedo = glm::vec3(0.8, 0.8, 0.0);
	materials.push_back(material_ground);

	glm::vec3 lookfrom(0, 0, 10);
	glm::vec3 lookat(0, 0, 0);
	glm::vec3 vup(0, 1, 0);
	float dist_to_focus = glm::length(lookfrom - lookat);
	float aperture = 2.0;
	Camera cam(lookfrom, lookat, vup, 45, 1.0, aperture, dist_to_focus);
	cameras.push_back(cam);
	mainCamera = &cameras[cameras.size() - 1];
}

Scene::~Scene() {
	delete rootObject;
	delete spaceTree;
}

void Scene::Update() {
	mainCamera->OnUpdate();
}

void Scene::CreateBoxScene() {
	glm::vec3 lookfrom(0, 0, 4);
	glm::vec3 lookat(0, 0, 0);
	glm::vec3 vup(0, 1, 0);
	float dist_to_focus = glm::length(lookfrom - lookat);
	float aperture = 2.0;
	Camera cam(lookfrom, lookat, vup, 45, 1.0, aperture, dist_to_focus);
	cameras.push_back(cam);
	mainCamera = &cameras[cameras.size() - 1];

	rootObject = new SceneObject("root");
	glm::vec3 box1Size = glm::vec3(0.5);
	SceneObject* cube = new SceneObject("Cube", Transform(), 1);
	cube->SetMesh(Mesh::CreateBox(box1Size));
	rootObject->AddChild(cube);

	UpdateObjectsWithMeshes();
}

void Scene::CreateTestScene() {
	glm::vec3 lookfrom(0, 0, 4);
	glm::vec3 lookat(0, 0, 0);
	glm::vec3 vup(0, 1, 0);
	float dist_to_focus = glm::length(lookfrom - lookat);
	float aperture = 2.0;
	Camera cam(lookfrom, lookat, vup, 20, 1.0f, aperture, dist_to_focus);
	cameras.push_back(cam);
	mainCamera = &cameras[cameras.size() - 1];
	skyColor = glm::vec3(0.5, 0.7, 1.0);

	Material testMaterial;
	testMaterial.albedo = glm::vec3(1.0);

	Material material_ground;
	material_ground.albedo = glm::vec3(0.8, 0.8, 0.0);

	Material material_center;
	material_center.albedo = glm::vec3(0.1, 0.2, 0.5);

	Material material_left;
	material_left.albedo = glm::vec3(0.8, 0.8, 0.8);
	material_left.metallic = 0.5;
	material_left.roughness = 0.3;

	Material material_right;
	material_right.albedo = glm::vec3(0.8, 0.6, 0.2);
	material_right.metallic = 0.3;

	Material material_glass;
	material_glass.albedo = glm::vec3(1.0);
	material_glass.transparency = 1.0;
	material_glass.roughness = 0.05;
	material_glass.refraction = 1.5;

	materials.push_back(testMaterial);
	materials.push_back(material_ground);
	materials.push_back(material_center);
	materials.push_back(material_left);
	materials.push_back(material_right);
	materials.push_back(material_glass);

	spheres.push_back(Sphere(glm::vec3(0.0, -100.5, 0.0), 100.0, 1));
	spheres.push_back(Sphere(glm::vec3(0.0, 0.0, 0.0), 0.5, 2));
	spheres.push_back(Sphere(glm::vec3(-1.0, 0.0, 0.0), -0.5, 5));
	spheres.push_back(Sphere(glm::vec3(1.0, 0.0, 0.0), 0.5, 4));
}

void Scene::CreateRandomScene() {
	glm::vec3 lookfrom(12, 2, 3);
	glm::vec3 lookat(0, 0, 0);
	glm::vec3 vup(0, 1, 0);
	float dist_to_focus = 10.0;
	float aperture = 0.1;
	Camera cam(lookfrom, lookat, vup, 20, 1.0, aperture, dist_to_focus);
	cameras.push_back(cam);
	mainCamera = &cameras[cameras.size() - 1];
	skyColor = glm::vec3(0.5, 0.7, 1.0);

	auto checker = std::make_shared<CheckerTexture>(glm::vec3(0.2, 0.3, 0.1), glm::vec3(0.9, 0.9, 0.9));
	Material checkerMaterial;
	checkerMaterial.albedo = glm::vec3(1.0f);
	checkerMaterial.texture = checker;
	checkerMaterial.roughness = 1.0f;
	spheres.push_back(Sphere(glm::vec3(0, -1000, 0), 1000, materials.size()));
	materials.push_back(checkerMaterial);

	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			auto choose_mat = RandomFloat();
			glm::vec3 center(a + 0.9 * RandomFloat(), 0.2, b + 0.9 * RandomFloat());

			if (glm::length(center - glm::vec3(4, 0.2, 0)) > 0.9) {
				Material sphere_material;

				if (choose_mat < 0.8) {
					sphere_material.albedo = RandomVector(0.0, 1.0) * RandomVector(0.0, 1.0);
					sphere_material.roughness = 1.0f;
					glm::vec3 center2 = center + glm::vec3(0, RandomFloat(0, 0.5), 0);
				}
				else if (choose_mat < 0.95) {
					sphere_material.albedo = RandomVector(0.5, 1);
					sphere_material.roughness = RandomFloat(0, 0.5);
					sphere_material.metallic = 0.5;
				}
				else {
					sphere_material.albedo = glm::vec3(1.0f);
					sphere_material.transparency = 1.0f;
					sphere_material.refraction = 1.7;
				}
				spheres.push_back(Sphere(center, 0.2, materials.size()));
				materials.push_back(sphere_material);
			}
		}
	}

	Material material1;
	material1.albedo = glm::vec3(1.0f);
	material1.transparency = 1.0f;
	material1.refraction = 1.7;
	spheres.push_back(Sphere(glm::vec3(0, 1, 0), 1.0, materials.size()));
	materials.push_back(material1);

	Material material2;
	material2.albedo = glm::vec3(0.4, 0.2, 0.1);
	material2.roughness = 1.0f;
	spheres.push_back(Sphere(glm::vec3(-4, 1, 0), 1.0, materials.size()));
	materials.push_back(material2);

	Material material3;
	material3.albedo = glm::vec3(0.7, 0.6, 0.5);
	material3.roughness = 0.0;
	material3.metallic = 0.5;
	spheres.push_back(Sphere(glm::vec3(4, 1, 0), 1.0, materials.size()));
	materials.push_back(material3);
}

void Scene::CreateCornellBoxScene() {
	glm::vec3 lookfrom(-10, 0, 0);
	glm::vec3 lookat(0, 0, 0);
	glm::vec3 vup(0, 1, 0);
	float fov = 40.0;
	float aspect = 1.0;
	auto dist_to_focus = 10.0;
	auto aperture = 0.0;
	Camera cam(lookfrom, lookat, vup, fov, aspect, aperture, dist_to_focus);
	cameras.push_back(cam);
	mainCamera = &cameras[cameras.size() - 1];

	rootObject = new SceneObject("root");

	/*
	Material red;
	red.albedo = glm::vec3(0.65, 0.05, 0.05);

	Material white;
	white.albedo = glm::vec3(0.73, 0.73, 0.73);

	Material green;
	green.albedo = glm::vec3(0.12, 0.45, 0.15);

	Material light;
	light.isLightSource = true;
	light.lightColor = glm::vec3(15.0);

	Material glass;
	glass.albedo = glm::vec3(1.0);
	glass.roughness = 0.0f;
	glass.transparency = 1.0f;
	glass.refraction = 1.7f;

	materials.push_back(red);
	materials.push_back(white);
	materials.push_back(green);
	materials.push_back(light);
	materials.push_back(glass);

	std::vector<glm::vec3> back;
	back.push_back(glm::vec3(0, 0, 555));
	back.push_back(glm::vec3(0, 555, 555));
	back.push_back(glm::vec3(555, 555, 555));
	back.push_back(glm::vec3(0, 0, 555));
	back.push_back(glm::vec3(555, 555, 555));
	back.push_back(glm::vec3(555, 0, 555));

	back.push_back(glm::vec3(0, 0, 0));
	back.push_back(glm::vec3(0, 0, 555));
	back.push_back(glm::vec3(555, 0, 555));
	back.push_back(glm::vec3(0, 0, 0));
	back.push_back(glm::vec3(555, 0, 555));
	back.push_back(glm::vec3(555, 0, 0));

	back.push_back(glm::vec3(0, 555, 0));
	back.push_back(glm::vec3(555, 555, 555));
	back.push_back(glm::vec3(0, 555, 555));
	back.push_back(glm::vec3(0, 555, 0));
	back.push_back(glm::vec3(555, 555, 0));
	back.push_back(glm::vec3(555, 555, 555));

	std::vector<glm::vec3> right;
	right.push_back(glm::vec3(0, 0, 0));
	right.push_back(glm::vec3(0, 555, 0));
	right.push_back(glm::vec3(0, 555, 555));
	right.push_back(glm::vec3(0, 0, 0));
	right.push_back(glm::vec3(0, 555, 555));
	right.push_back(glm::vec3(0, 0, 555));

	std::vector<glm::vec3> left;
	left.push_back(glm::vec3(555, 0, 0));
	left.push_back(glm::vec3(555, 555, 555));
	left.push_back(glm::vec3(555, 555, 0));
	left.push_back(glm::vec3(555, 0, 0));
	left.push_back(glm::vec3(555, 0, 555));
	left.push_back(glm::vec3(555, 555, 555));

	std::vector<glm::vec3> topLight;
	topLight.push_back(glm::vec3(213, 554, 227));
	topLight.push_back(glm::vec3(213, 554, 332));
	topLight.push_back(glm::vec3(343, 554, 332));
	topLight.push_back(glm::vec3(213, 554, 227));
	topLight.push_back(glm::vec3(343, 554, 322));
	topLight.push_back(glm::vec3(343, 554, 227));

	SceneObject* ww = new SceneObject("White Walls", Transform(), 2);
	ww->SetMesh(Mesh(back));
	SceneObject* gw = new SceneObject("Green Wall", Transform(), 1);
	gw->SetMesh(Mesh(left));
	SceneObject* rw = new SceneObject("Red Wall", Transform(), 3);
	rw->SetMesh(Mesh(right));
	SceneObject* ls = new SceneObject("Light Source", Transform(), 4);
	ls->SetMesh(Mesh(topLight));
	rootObject->AddChild(ww);
	rootObject->AddChild(gw);
	rootObject->AddChild(rw);
	rootObject->AddChild(ls);
	lights.push_back(ls);

	glm::vec3 box1Size = glm::vec3(165);
	glm::vec3 box2Size = glm::vec3(165, 330, 165);

	SceneObject* cube = new SceneObject("Cube", Transform(glm::vec3(130, 0, 65) + box1Size / 2.0f, glm::vec3(0, glm::radians(-18.0), 0)), 2);
	cube->SetMesh(Mesh::CreateBox(box1Size));
	rootObject->AddChild(cube);

	SceneObject* box = new SceneObject("Cube", Transform(glm::vec3(265, 0, 295) + box2Size / 2.0f, glm::vec3(0, glm::radians(15.0), 0)), 2);
	box->SetMesh(Mesh::CreateBox(box2Size));
	rootObject->AddChild(box);

	*/

	SceneObject* torus = SceneLoader::LoadObject("../models/cornellbox.obj", this);
	rootObject->AddChild(torus);

	//spheres.push_back(Sphere(glm::vec3(450, 75, 120), 75, 5));

	UpdateObjectsWithMeshes();

	for (int32_t i = 0; i < objectsWithMeshes.size(); i++) {
		if (materials[objectsWithMeshes[i]->GetMaterial()].lightColor != glm::vec3(0)) {
			lights.push_back(objectsWithMeshes[i]);
		}
	}
}

bool Scene::CheckCollision(Ray ray, float tMin, float tMax, CollisionInfo& outCollision) const {
	if(spaceTree) outCollision = spaceTree->CheckCollision(ray, tMin, tMax);
	if (outCollision.collided) {
		tMax = outCollision.distance;
	}
	for (int32_t i = 0; i < spheres.size(); i++) {
		if (spheres[i].TestCollision(ray, tMin, tMax, outCollision)) {
			tMax = outCollision.distance;
		}
	}
	return outCollision.collided;
}

const std::vector<SceneObject*>& Scene::GetObjectWithMeshes() const {
	return objectsWithMeshes;
}

void Scene::UpdateObjectsWithMeshes() {
	if(spaceTree) delete spaceTree;
	objectsWithMeshes.clear();
	FillObjectsWithMeshesList(rootObject);
	spaceTree = new SpaceTreeNode(objectsWithMeshes);
}

void Scene::FillObjectsWithMeshesList(SceneObject* obj) {
	if (obj->GetMesh().vertices.size() > 0)
		objectsWithMeshes.push_back(obj);
	for(int32_t i = 0; i < obj->children.size(); i++)
		FillObjectsWithMeshesList(obj->children[i]);
}