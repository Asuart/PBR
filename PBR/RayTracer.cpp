#include "pch.h"
#include "RayTracer.h"

RayTracer::RayTracer(int32_t _width, int32_t _height, int32_t _maxBounces) 
	: film(new Film(_width, _height)), maxBounces(_maxBounces) {}

RayTracer::~RayTracer() {
	delete film;
}

void RayTracer::SetCamera(Camera* cam) {
	camera = cam;
	if(film != nullptr)
		film->Reset();
}

void RayTracer::SetScene(Scene* s) {
	scene = s;
	if (film != nullptr)
		film->Reset();
}

void RayTracer::DrawQuad() const {
	if (film != nullptr)
		film->DrawQuad();
}