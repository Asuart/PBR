#pragma once
#include "Renderer.h"
#include "PDF.h"
#include "Sampler.h"

class Sampler;

class CPURayTracer : public Renderer {
public:
	CPURayTracer(int32_t width, int32_t height, bool accumulate = true);
	~CPURayTracer();

	virtual void Reset() override;
	virtual void SetScene(const Scene& scene) override;
	virtual void SetCamera(const Camera& camera) override;
	virtual void Resize(int32_t width, int32_t height) override;
	virtual void RenderSample() override;
	virtual void StartRender() override;
	virtual void EndRender() override;
	virtual void DrawFrame() override;
	virtual float GetProgress() const override;

private:

	Sampler* sampler;


	std::thread renderThread;
	bool isRendering;

	glm::vec3* pixelData;
	glm::vec3* pixelAccumulator;

	void PerPixel(uint32_t x, uint32_t y) const;
	glm::vec3 TraceRay(Ray ray, int32_t maxDepth, glm::vec3 throughput = glm::vec3(1.0)) const;

	friend class Sampler;
};