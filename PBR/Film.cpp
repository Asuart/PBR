#include "pch.h"
#include "Film.h"

Film::Film(int32_t _width, int32_t _height)
	: width(_width), height(_height), pixelSize(glm::vec2(1.0 / _width, 1.0 / _height)) {
	pixelAccumulator = new glm::vec4[width * height];
}

Film::~Film() {
	delete[] pixelAccumulator;
}

void Film::Reset() {
	for (int32_t i = 0; i < width * height; i++) {
		pixelAccumulator[i] = glm::vec4(0);
	}
}

void Film::SetPixel(int32_t x, int32_t y, glm::vec4 color) {
	pixelAccumulator[y * width + x] = color;
}

void Film::AddPixel(int32_t x, int32_t y, glm::vec4 color) {
	pixelAccumulator[y * width + x] += color;
}

glm::vec2 Film::GetUV(int32_t x, int32_t y) const {
	return glm::vec2((float)x / width, (float)y / height) + pixelSize * glm::vec2(RandomFloat(), RandomFloat());
}


void Film::SetupRenderQuad(const glm::vec2& min, const glm::vec2& max) {
	glGenTextures(1, &quadTexture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, quadTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT,
		NULL);
	glBindImageTexture(0, quadTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	glm::vec3 vertices[6]{
		glm::vec3(min.x, min.y, 0),
		glm::vec3(min.x, max.y, 0),
		glm::vec3(max.x, max.y, 0),
		glm::vec3(min.x, min.y, 0),
		glm::vec3(max.x, max.y, 0),
		glm::vec3(max.x, min.y, 0)
	};

	glGenVertexArrays(1, &quadVAO);
	glBindVertexArray(quadVAO);

	glGenBuffers(1, &quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 6, vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	quadProgram = LoadShader("C:/Users/User/Desktop/PBR/PBR/QuadVertexShader.glsl", "C:/Users/User/Desktop/PBR/PBR/QuadFragmentShader.glsl");

	glBindVertexArray(0);

	quadReady = true;
}

void Film::DrawQuad() const {
	if (!quadReady)
		return;
	glUseProgram(quadProgram);
	glBindVertexArray(quadVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, quadTexture);
	glm::vec4* data = new glm::vec4[width * height];
	for (uint32_t i = 0; i < width * height; i++) {
		float scale = 1.0 / pixelAccumulator[i].w;
		data[i] = glm::clamp(glm::sqrt(pixelAccumulator[i] * scale), 0.0f, 0.999f);
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, data);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	delete[] data;
}