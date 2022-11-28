#include "pch.h"
#include "CPURayTracer.h"

CPURayTracer::CPURayTracer(int32_t width, int32_t height, bool accumulate) {
	Resize(width, height);
	SetAccumulate(accumulate);
	Reset();
	SetupRenderQuad();
	isRendering = false;
	maxBounces = 1;
}

CPURayTracer::~CPURayTracer() {
	renderThread.detach();
	delete[] pixelData;
	delete[] pixelAccumulator;
}

void CPURayTracer::Reset() {
	sample = 0;
	currentPixel = 0;
	for (uint32_t i = 0; i < width * height; i++) {
		pixelData[i] = glm::vec3(0);
		pixelAccumulator[i] = glm::vec3(0);
	}
}

void CPURayTracer::SetScene(const Scene& scene) {
	activeScene = &scene;
}

void CPURayTracer::SetCamera(const Camera& camera) {
	activeCamera = &camera;
}

void CPURayTracer::Resize(int32_t _width, int32_t _height) {
	delete[] pixelData;
	delete[] pixelAccumulator;
	width = _width;
	height = _height;
	pixelData = new glm::vec3[width * height];
	pixelAccumulator = new glm::vec3[width * height];
	pixelSize = glm::vec2(1.0f / width, 1.0f / height);
}

void CPURayTracer::RenderSample() {
	Time::MeasureStart("Time to sample");
	currentPixel = 0;
	for (uint32_t y = 0; y < height; y++) {
		for (uint32_t x = 0; x < width; x++, currentPixel++) {
			if (accumulate) pixelAccumulator[currentPixel] += PerPixel(x, y);
			else pixelAccumulator[currentPixel] = PerPixel(x, y);
		}
	}
	if (accumulate) sample++;
	frameReady = true;
	Time::MeasureEnd("Time to sample");
}

void CPURayTracer::StartRender() {
	isRendering = true;
	renderThread = std::thread([&]() {
		for (; isRendering;) {
			if (frameReady) {
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
				continue;
			}
			RenderSample();
		}
		});
}

void CPURayTracer::EndRender() {
	isRendering = false;
	renderThread.join();
}

void CPURayTracer::DrawFrame() {
	glUseProgram(quadProgram);
	glBindVertexArray(quadVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, quadTexture);

	for (uint32_t i = 0; i < width * height; i++) {
		float scale = 1.0 / sample;
		pixelData[i] = glm::clamp(glm::sqrt(pixelAccumulator[i] * scale), 0.0f, 0.999f);
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_FLOAT, pixelData);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	frameReady = false;
}

glm::vec3 CPURayTracer::PerPixel(uint32_t x, uint32_t y) {
	glm::vec2 uv = glm::vec2((float)x / width, (float)y / height) + pixelSize * glm::vec2(RandomFloat(), RandomFloat());
	Ray ray = activeCamera->GetRay(uv);
	return TraceRay(ray, maxBounces);
}

glm::vec3 CPURayTracer::TraceRay(Ray ray, int32_t maxDepth) {
	const glm::vec3 skyboxColor = glm::vec3(0);
	glm::vec3 resultColor = glm::vec3(1.0);

	CollisionInfo collision;
	for (uint32_t i = 0; i < maxDepth; i++) {
		activeScene->CheckCollision(ray, 0.001, 10000.0, collision);
		if (!collision.collided)
			return skyboxColor;

		const Material& material = activeScene->materials[collision.materialIndex % activeScene->materials.size()];

		if (material.isLightSource) {
			resultColor *= material.lightColor;
			break;
		}

		glm::vec3 color;
		Ray scattered;
		float pdf;
		material.Scatter(ray, collision, color, scattered, pdf);

		if (RandomFloat() > 0.8) {
			scattered.direction += glm::normalize(scattered.direction - collision.position + glm::vec3(277, 555, 277));
		}

		resultColor *= color * material.ScatteringPdf(ray, collision, scattered) / pdf;
		ray = scattered;
	}

	return resultColor;
}

float CPURayTracer::GetProgress() const {
	return (float)currentPixel / (width * height - 1);
}