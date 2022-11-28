#pragma once
#include "pch.h"
#include "Random.h"
#include "Scene.h"
#include "ShaderLibrary.h"

class Renderer {
public:
	virtual void Reset() = 0;
	virtual void SetScene(const Scene& scene) = 0;
	virtual void SetCamera(const Camera& camera) = 0;
	virtual void Resize(int32_t width, int32_t height) = 0;
	virtual void RenderSample() = 0;
	virtual void StartRender() = 0;
	virtual void EndRender() = 0;
	virtual void DrawFrame() = 0;

	virtual float GetProgress() const = 0;

	virtual void SetupRenderQuad(const glm::vec2& min = glm::vec2(-1.0), const glm::vec2& max = glm::vec2(1.0));

	virtual uint32_t CurrentSample() const;
	virtual bool FrameIsReady() const;
	virtual void SetAccumulate(bool state);
	virtual void SetFrameReady(bool state);
protected:
	uint32_t width, height;
	glm::vec2 pixelSize;
	int32_t sample;
	uint32_t maxBounces;
	bool accumulate;
	bool frameReady;

	const Scene* activeScene;
	const Camera* activeCamera;

	GLuint quadVAO;
	GLuint quadVBO;
	GLuint quadTexture;
	GLuint quadProgram;
};
