#pragma once
#include "Renderer.h"

class GPURayTracer : public Renderer {
public:
	GPURayTracer(int32_t width, int32_t height, bool accumulate = true);
	~GPURayTracer();

	virtual void Reset() override;
	virtual void SetScene(const Scene& scene) override;
	virtual void SetCamera(const Camera& camera) override;
	virtual void Resize(int32_t width, int32_t height) override;
	virtual void RenderSample() override;
	virtual void DrawFrame() override;

	virtual void StartRender() override;
	virtual void EndRender() override;

private:
	GLuint rayProgram;

	int32_t trianglesCount;
	int32_t* trianglesBufferData;
	GLuint trianglesBuffer;

	GLuint hitResultsBuffer;
};

