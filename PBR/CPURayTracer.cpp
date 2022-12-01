#include "pch.h"
#include "CPURayTracer.h"

CPURayTracer::CPURayTracer(int32_t width, int32_t height, bool accumulate) {
	Resize(width, height);
	SetAccumulate(accumulate);
	Reset();
	SetupRenderQuad();
	isRendering = false;
	maxBounces = 4;
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
	return TraceRay(ray, maxBounces, glm::vec3(1.0));
}


inline float MaxComponent(const glm::vec3& v) {
	return glm::max(v.x, glm::max(v.y, v.z));
}

glm::vec3 CPURayTracer::TraceRay(Ray ray, int32_t maxDepth, glm::vec3 throughput) {
	if (maxDepth == 0)
		return glm::vec3(0);

	CollisionInfo collision;
	activeScene->CheckCollision(ray, 0.001, 10000.0, collision);

	if (!collision.collided)
		return activeScene->skyColor;

	const Material& material = activeScene->materials[collision.materialIndex];
	glm::vec3 f = glm::vec3(0.0f);

	//float terminateProb = MaxComponent(throughput);
	//if (RandomFloat() > terminateProb)
	//	return f;

	Ray scatteredRay;
	glm::vec3 brdfMultiplier = material.Sample(ray, collision, scatteredRay);
	//throughput *= brdfMultiplier / terminateProb;

	f += TraceRay(scatteredRay, maxDepth - 1, throughput) * brdfMultiplier;

	for (int32_t i = 0; i < activeScene->lights.size(); i++) {
		glm::vec3 onLight = activeScene->lights[i]->GetMesh().RandomInnerPoint();
		glm::vec3 toLight = onLight - collision.position;
		float lightDistance2 = glm::length(toLight) * glm::length(toLight);
		toLight = glm::normalize(toLight);
		if (glm::dot(collision.normal, toLight) < 0.001)
			continue;
		float lightArea = activeScene->lights[i]->GetMesh().area;
		float pdf = (lightArea * fabs(glm::dot(collision.normal, toLight))) / lightDistance2;
		Ray lightRay(collision.position, toLight);
		activeScene->CheckCollision(lightRay, 0.001, 10000.0, collision);
		if (!collision.collided)
			continue;
		const Material& lightMaterial = activeScene->materials[collision.materialIndex];
		f += (material.albedo) * (lightMaterial.lightColor * pdf);
	}

	return f;

	//glm::vec3 indirectLight = TraceRay(scattered, maxDepth - 1, throughput) / pdf;


	/*
	glm::vec3 resultColor = glm::vec3(1.0);
	CollisionInfo collision;
	for (uint32_t i = 0; i < maxDepth; i++) {
		activeScene->CheckCollision(ray, 0.001, 10000.0, collision);
		if (!collision.collided)
			return resultColor * skyboxColor;

		const Material& material = activeScene->materials[collision.materialIndex % activeScene->materials.size()];

		if (material.isLightSource) {
			resultColor *= material.lightColor;
			break;
		}

		glm::vec3 color;
		Ray scattered;
		float pdf;
		material.Scatter(ray, collision, color, scattered, pdf);

		resultColor *= color * material.ScatteringPdf(ray, collision, scattered) / pdf;
		ray = scattered;
	}
	*/
}

float CPURayTracer::GetProgress() const {
	return (float)currentPixel / (width * height - 1);
}