#include "pch.h"
#include "Renderer.h"

void Renderer::SetupRenderQuad(const glm::vec2& min, const glm::vec2& max) {
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
}

uint32_t Renderer::CurrentSample() const {
	return sample;
}

bool Renderer::FrameIsReady() const {
	return frameReady;
}

void Renderer::SetAccumulate(bool state) {
	accumulate = state;
}

void Renderer::SetFrameReady(bool state) {
	frameReady = state;
}