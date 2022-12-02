#pragma once
#include "pch.h"
#include "CPURayTracer.h"

class CPURayTracer;

struct SamplerQuad {
	glm::ivec2 start, end;
	SamplerQuad(glm::ivec2 start, glm::ivec2 end);
};

class Sampler {
public:
	const CPURayTracer* renderer;
	std::vector<SamplerQuad> quads;
	std::vector<std::thread*> renderThreads;


	int32_t hQuads;
	int32_t vQuads;
	glm::ivec2 size;

	Sampler(const CPURayTracer* _renderer, int32_t _hQuads, int32_t _vQuads);
	~Sampler();

	void Dispatch();
	void SampleQuad(int32_t i);
};

