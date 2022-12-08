#pragma once
#include "RayTracer.h"

class GPURayTracer : public RayTracer {
public:
	GPURayTracer(int32_t width, int32_t height, bool accumulate = true);
	~GPURayTracer();

	virtual void SetScene(Scene* s) override;
	virtual void StartRender() override;
	virtual void EndRender() override;

private:
	GLuint rayProgram;

	int32_t trianglesCount;
	int32_t* trianglesBufferData;
	GLuint trianglesBuffer;

	int32_t bvhNodesCount, bvhLeafsCount;
	GLuint bvhNodesBuffer, bvhLeafsBuffer;
	int32_t* bvhNodesBufferData, bvhLeafsBufferData;

	GLuint hitResultsBuffer;
};

