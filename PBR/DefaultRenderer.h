#pragma once
#include "Renderer.h"

struct ObjectOnGPU {
	GLuint vao;
	GLuint vertexBuffer;
	GLuint indicesBuffer;
	uint32_t trianglesCount;
};

class DefaultRenderer : public Renderer {
public:
	DefaultRenderer();
	~DefaultRenderer();

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
	GLuint defaultShaderProgram;
	std::vector<ObjectOnGPU> objects;

	void FreeGPU();
};

