#pragma once
#include "pch.h"
#include "Random.h"
#include "Scene.h"
#include "ShaderLibrary.h"
#include "Film.h"

class Renderer {
public:
	Film* film;
	int32_t sample = 0;

	virtual void Reset() = 0;
	virtual void SetScene(const Scene& scene) = 0;
	virtual void SetCamera(const Camera& camera) = 0;
	virtual void Resize(int32_t width, int32_t height) = 0;
	virtual void RenderSample() = 0;
	virtual void StartRender() = 0;
	virtual void EndRender() = 0;
	virtual void DrawFrame() = 0;


	virtual bool FrameIsReady() const;
	virtual void SetAccumulate(bool state);
	virtual void SetFrameReady(bool state);
protected:
	uint32_t width, height;
	uint32_t maxBounces;
	bool accumulate;
	bool frameReady;

	const Scene* activeScene;
	const Camera* activeCamera;
};
