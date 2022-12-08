#include "pch.h"
#include "RendererWindow.h"

void window_size_callback(GLFWwindow* window, int width, int height) {
	int32_t min = glm::min(width, height);
	glViewport(0, 0, min, min);
}

RendererWindow::RendererWindow(int32_t width, int32_t height)
	: m_width(width), m_height(height), rayTracer(nullptr)
{
	InitWindow();
	InitGL();

	rayTracer = new CPURayTracer(width, height);
	//m_renderer = new DefaultRenderer();

	glfwSetWindowSizeCallback(m_mainWindow, window_size_callback);
	UserInput::SetInputWindow(m_mainWindow);
}

RendererWindow::~RendererWindow() {
	delete rayTracer;
	delete m_scene;
	glfwDestroyWindow(m_mainWindow);
	glfwTerminate();
}

void RendererWindow::Start() {
	m_scene = new Scene();

	//m_scene->CreateBoxScene();
	//m_scene->CreateTestScene();
	m_scene->CreateCornellBoxScene();
	//m_scene->CreateRandomScene();
	//m_scene = SceneLoader::LoadScene("D:/repositories/MathVisualiser/ModelLoader/dae/vampire/dancing_vampire.dae");
	//m_scene = SceneLoader::LoadScene("D:/repositories/MathVisualiser/ModelLoader/dae/spider.dae");

	//Animation danceAnimation("D:/repositories/MathVisualiser/ModelLoader/dae/vampire/dancing_vampire.dae", m_scene->objectsWithMeshes[0]);
	//Animator animator(&danceAnimation);

	rayTracer->SetScene(m_scene);
	rayTracer->SetCamera(m_scene->mainCamera);
	rayTracer->StartRender();

	float timeAccumulator = 0.0;

	float deltaTime = 0;
	float lastFrame = 0;

	while (!glfwWindowShouldClose(m_mainWindow)) {

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		//animator.UpdateAnimation(deltaTime);

		//auto transforms = animator.GetFinalBoneMatrices();
		//for (int32_t i = 0; i < transforms.size(); ++i)
		//	glUniformMatrix4fv(glGetUniformLocation(6, std::string("finalBonesMatrices[" + std::to_string(i) + "]").c_str()), 1, GL_FALSE, &transforms[i][0][0]);

		OnUpdate();
		
		timeAccumulator += Time::deltaTime;
		if (timeAccumulator > 0.1) {

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			rayTracer->DrawQuad();
			glfwSwapBuffers(m_mainWindow);


			timeAccumulator = 0;
			std::string newTitle = "Sample: " + std::to_string(rayTracer->sample);
			glfwSetWindowTitle(m_mainWindow, newTitle.c_str());
		}

		Time::Update();
		UserInput::Reset();
		glfwPollEvents();
	}

	rayTracer->EndRender();
}

void RendererWindow::OnUpdate() {
	m_scene->Update();
}

void RendererWindow::InitWindow() {
	if (glfwInit() == 0) {
		Logger::Error("GLFW initialization failed.");
		exit(1);
	}
	m_mainWindow = glfwCreateWindow(m_width, m_height, "PBR main window", NULL, NULL);
	glfwMakeContextCurrent(m_mainWindow);
}

void RendererWindow::InitGL() {
	if (glewInit() != GLEW_OK) {
		Logger::Error("GLEW initialization failed.");
		exit(1);
	}
	glEnable(GL_DEPTH_TEST);
}

