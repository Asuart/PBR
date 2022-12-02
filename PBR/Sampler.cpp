#include "pch.h"
#include "Sampler.h"

SamplerQuad::SamplerQuad(glm::ivec2 _start, glm::ivec2 _end) 
	: start(_start), end(_end) {}

Sampler::Sampler(const CPURayTracer* _renderer, int32_t _hQuads, int32_t _vQuads) 
	: renderer(_renderer), hQuads(_hQuads), vQuads(_vQuads)
{
	size = renderer->GetTextureSize();
	glm::ivec2 quadSize = size / glm::ivec2(hQuads, vQuads);
	glm::ivec2 align = size % quadSize;
	for (int32_t y = 0; y < vQuads; y++) {
		for (int32_t x = 0; x < hQuads; x++) {
			glm::ivec2 start = quadSize * glm::ivec2(x, y);
			glm::ivec2 end = quadSize * glm::ivec2(x + 1, y + 1);
			if ((x + 1) == hQuads)
				end.x += align.x;
			if ((y + 1) == vQuads)
				end.y += align.y;
			quads.push_back(SamplerQuad(start, end));
		}
	}
}

Sampler::~Sampler() {}

void Sampler::Dispatch() {
	for (int32_t i = 0; i < quads.size(); i++) {
		renderThreads.push_back(new std::thread(std::thread([&, i]() {
			SampleQuad(i);
			})));
	}

	for (int32_t i = 0; i < renderThreads.size(); i++) {
		renderThreads[i]->join();
	}
	for (int32_t i = 0; i < renderThreads.size(); i++) {
		delete renderThreads[i];
	}
	renderThreads.clear();
}

void Sampler::SampleQuad(int32_t i) {
	SamplerQuad quad = quads[i];
	for (int32_t y = quad.start.y; y < quad.end.y; y++) {
		for (int32_t x = quad.start.x; x < quad.end.x; x++) {
			renderer->PerPixel(x, y);
		}
	}
}