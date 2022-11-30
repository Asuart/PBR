#pragma once
#include "Renderer.h"
#include "PDF.h"

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
	std::thread renderThread;
	bool isRendering;

	uint32_t currentPixel;
	glm::vec3* pixelData;
	glm::vec3* pixelAccumulator;

	inline glm::vec3 PerPixel(uint32_t x, uint32_t y);
	glm::vec3 TraceRay(Ray ray, int32_t maxDepth, glm::vec3 throughput = glm::vec3(1.0));
};