#pragma once
#include "pch.h"
#include "UserInput.h"
#include "Logger.h"
#include "Renderer.h"
#include "Time.h"
#include "ShaderLibrary.h"
#include "CPURayTracer.h"
#include "GPURayTracer.h"
#include "DefaultRenderer.h"
#include "SceneLoader.h"
#include "Animation.h"
#include "Animator.h"

class RendererWindow {
public:
	RendererWindow(int32_t width = 800, int32_t height = 800);
	~RendererWindow();

	void Start();
	void OnUpdate();

private:
	Renderer* m_renderer;
	Scene* m_scene;

	GLFWwindow* m_mainWindow;
	int32_t m_width;
	int32_t m_height;

	void InitWindow();
	void InitGL();
};

