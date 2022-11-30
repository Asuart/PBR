#include "pch.h"
#include "Scene.h"

Scene::Scene() : spaceTree(nullptr), rootObject(nullptr) {
	Material material_ground;
	material_ground.albedo = glm::vec3(0.8, 0.8, 0.0);
	materials.push_back(material_ground);

	glm::vec3 lookfrom(0, 0, 10);
	glm::vec3 lookat(0, 0, 0);
	glm::vec3 vup(0, 1, 0);
	auto dist_to_focus = glm::length(lookfrom - lookat);
	auto aperture = 2.0;
	Camera cam(lookfrom, lookat, vup, 45, 1.0, aperture, dist_to_focus);
	cameras.push_back(cam);
	mainCamera = &cameras[0];
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
	auto dist_to_focus = glm::length(lookfrom - lookat);
	auto aperture = 2.0;
	Camera cam(lookfrom, lookat, vup, 45, 1.0, aperture, dist_to_focus);
	cameras.push_back(cam);
	mainCamera = &cameras[0];

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
	auto dist_to_focus = glm::length(lookfrom - lookat);
	auto aperture = 2.0;
	Camera cam(lookfrom, lookat, vup, 20, 16.0f / 9.0f, aperture, dist_to_focus);
	cameras.push_back(cam);
	mainCamera = &cameras[cameras.size() - 1];

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
	/*
	glm::vec3 lookfrom(12, 2, 3);
	glm::vec3 lookat(0, 0, 0);
	glm::vec3 vup(0, 1, 0);
	auto dist_to_focus = 10.0;
	auto aperture = 0.1;
	Camera cam(lookfrom, lookat, vup, 20, 16.0f / 9.0f, aperture, dist_to_focus);
	mainCamera = &cam;

	auto checker = std::make_shared<CheckerTexture>(glm::vec3(0.2, 0.3, 0.1), glm::vec3(0.9, 0.9, 0.9));
	materials.push_back(Material(glm::vec3(1.0f), checker));
	m_objects.push_back(new Sphere(glm::vec3(0, -1000, 0), 1000, 0));

	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			auto choose_mat = RandomFloat();
			glm::vec3 center(a + 0.9 * RandomFloat(), 0.2, b + 0.9 * RandomFloat());

			if (glm::length(center - glm::vec3(4, 0.2, 0)) > 0.9) {
				Material* sphere_material;

				if (choose_mat < 0.8) {
					// diffuse
					auto albedo = RandomVector(0.0, 1.0) * RandomVector(0.0, 1.0);
					sphere_material = new Lambertian(albedo);
					auto center2 = center + glm::vec3(0, RandomFloat(0, 0.5), 0);
					m_objects.push_back(new MovingSphere(center, center2, 0.0, 1.0, 0.2, m_materials.size()));
					m_materials.push_back(sphere_material);
				}
				else if (choose_mat < 0.95) {
					// metal
					auto albedo = RandomVector(0.5, 1);
					auto fuzz = RandomFloat(0, 0.5);
					sphere_material = new Metal(albedo, fuzz);
					m_objects.push_back(new Sphere(center, 0.2, m_materials.size()));
					m_materials.push_back(sphere_material);
				}
				else {
					// glass
					sphere_material = new Glass(glm::vec3(1.0), 0.0, 1.5);
					m_objects.push_back(new Sphere(center, 0.2, m_materials.size()));
					m_materials.push_back(sphere_material);
				}
			}
		}
	}

	auto material1 = new Glass(glm::vec3(1.0), 0, 1.5);
	m_objects.push_back(new Sphere(glm::vec3(0, 1, 0), 1.0, m_materials.size()));
	m_materials.push_back(material1);

	auto material2 = new Lambertian(glm::vec3(0.4, 0.2, 0.1));
	m_objects.push_back(new Sphere(glm::vec3(-4, 1, 0), 1.0, m_materials.size()));
	m_materials.push_back(material2);

	auto material3 =new Metal(glm::vec3(0.7, 0.6, 0.5), 0.0);
	m_objects.push_back(new Sphere(glm::vec3(4, 1, 0), 1.0, m_materials.size()));
	m_materials.push_back(material3);
	*/
}

void Scene::CreateCornellBoxScene() {
	glm::vec3 lookfrom(277, 277, -800);
	glm::vec3 lookat(277, 277, 0);
	glm::vec3 vup(0, 1, 0);
	float fov = 40.0;
	float aspect = 1.0;
	auto dist_to_focus = 10.0;
	auto aperture = 0.0;
	Camera cam(lookfrom, lookat, vup, fov, aspect, aperture, dist_to_focus);
	cameras.push_back(cam);
	mainCamera = &cameras[cameras.size() - 1];

	Material red;
	red.albedo = glm::vec3(0.65, 0.05, 0.05);

	Material white;
	white.albedo = glm::vec3(0.73, 0.73, 0.73);

	Material green;
	green.albedo = glm::vec3(0.12, 0.45, 0.15);

	Material light;
	light.isLightSource = true;
	light.lightColor = glm::vec3(15.0);

	materials.push_back(red);
	materials.push_back(white);
	materials.push_back(green);
	materials.push_back(light);

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

	rootObject = new SceneObject("root");
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


	//SceneObject* spider = SceneLoader::LoadObject("D:/repositories/MathVisualiser/ModelLoader/dae/spider.dae");
	//spider->SetMaterial(2);
	//spider->SetTransform(Transform(glm::vec3(270, 270, 270), glm::vec3(0), glm::vec3(15)));
	//rootObject->AddChild(spider);

	SceneObject* stalin = SceneLoader::LoadObject("D:/repositories/MathVisualiser/ModelLoader/untitled.stl");
	stalin->SetMaterial(2);
	stalin->SetTransform(Transform(glm::vec3(210, 150, 270), glm::vec3(0, PI, 0), glm::vec3(2.2)));
	rootObject->AddChild(stalin);

	UpdateObjectsWithMeshes();
}

bool Scene::CheckCollision(Ray ray, float tMin, float tMax, CollisionInfo& outCollision) const {
	/*
	CollisionInfo col;
	bool hitAnything = false;
	for (uint32_t i = 0; i < objectsWithMeshes.size(); i++) {
		if (objectsWithMeshes[i]->TestCollision(ray, tMin, tMax, col)) {
			hitAnything = true;
			tMax = col.distance;
			outCollision = col;
		}
	}
	return hitAnything;
	*/
	
	outCollision = spaceTree->CheckCollision(ray, tMin, tMax);
	return outCollision.collided;
}

const std::vector<SceneObject*>& Scene::GetObjectWithMeshes() const {
	return objectsWithMeshes;
}

void Scene::UpdateObjectsWithMeshes() {
	delete spaceTree;
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