#pragma once
#include "RayTracer.h"
#include "PDF.h"
#include "Sampler.h"

class Sampler;

class CPURayTracer : public RayTracer {
public:

	CPURayTracer(int32_t width, int32_t height, int32_t _maxBounces = 12);
	~CPURayTracer();

	virtual void StartRender() override;
	virtual void EndRender() override;

private:
	Sampler* sampler;
	bool isRendering = false;

	void PerPixel(uint32_t x, uint32_t y) const;
	glm::vec3 TraceRay(Ray ray, int32_t maxDepth, glm::vec3 throughput = glm::vec3(1.0)) const;

	friend class Sampler;
};