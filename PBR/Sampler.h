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
	int32_t threadsCount = 6;
	int32_t samplesPerPixel = 1;
	CPURayTracer* renderer;
	std::vector<SamplerQuad> quads;
	std::vector<std::thread*> renderThreads;

	std::queue<int32_t> quadQueue;
	std::mutex quadQueueMutex;


	int32_t hQuads;
	int32_t vQuads;
	glm::ivec2 size;

	Sampler(CPURayTracer* _renderer, int32_t _hQuads, int32_t _vQuads);
	~Sampler();

	void Dispatch();
	void Join();
	void SampleQuad();
};

