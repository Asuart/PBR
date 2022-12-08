#pragma once
#include "pch.h"
#include "Random.h"
#include "ShaderLibrary.h"

class Film {
public:
	const int32_t width;
	const int32_t height;
	const glm::vec2 pixelSize;
	glm::vec4* pixelAccumulator;

	Film(int32_t _width, int32_t _height);
	~Film();

	void Reset();
	void SetPixel(int32_t x, int32_t y, glm::vec4 color);
	void AddPixel(int32_t x, int32_t y, glm::vec4 color);

	glm::vec2 GetUV(int32_t x, int32_t y) const;

	void SetupRenderQuad(const glm::vec2& min = glm::vec2(-1.0), const glm::vec2& max = glm::vec2(1.0));
	void DrawQuad() const;
private:
	bool quadReady = false;
	GLuint quadVAO = 0;
	GLuint quadVBO = 0;
	GLuint quadTexture = 0;
	GLuint quadProgram = 0;
};

