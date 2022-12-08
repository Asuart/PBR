#pragma once
#include "Scene.h"
#include "Film.h"

class RayTracer {
public:
	Film* film;
	Scene* scene;
	Camera* camera;
	int32_t maxBounces;
	int32_t sample = 0;

	RayTracer(int32_t _width, int32_t _height, int32_t _maxBounces);
	~RayTracer();

	virtual void SetCamera(Camera* cam);
	virtual void SetScene(Scene* scene);
	virtual void StartRender() = 0;
	virtual void EndRender() = 0;

	virtual void DrawQuad() const;
private:
};

