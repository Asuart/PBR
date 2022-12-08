#include "pch.h"
#include "CPURayTracer.h"

CPURayTracer::CPURayTracer(int32_t width, int32_t height, int32_t _maxBounces)
	: RayTracer(width, height, _maxBounces) {
	
	film->SetupRenderQuad(glm::vec2(-1.0), glm::vec2(1.0));
	sampler = new Sampler(this, 8, 8);
}

CPURayTracer::~CPURayTracer() {
	delete sampler;
}

void CPURayTracer::StartRender() {
	isRendering = true;
	sampler->Dispatch();
}

void CPURayTracer::EndRender() {
	isRendering = false;
	sampler->Join();
}

void CPURayTracer::PerPixel(uint32_t x, uint32_t y) const {
	glm::vec2 uv = film->GetUV(x, y);
	Ray ray = camera->GetRay(uv);
	glm::vec3 color = TraceRay(ray, maxBounces, glm::vec3(1.0));
	film->AddPixel(x, y, glm::vec4(color, 1.0));
}

inline float MaxComponent(const glm::vec3& v) {
	return glm::max(v.x, glm::max(v.y, v.z));
}

glm::vec3 CPURayTracer::TraceRay(Ray ray, int32_t maxDepth, glm::vec3 throughput) const {
	if (maxDepth == 0)
		return glm::vec3(0);

	CollisionInfo collision;
	scene->CheckCollision(ray, 0.001, 10000.0, collision);

	if (!collision.collided)
		return scene->skyColor;

	const Material& material = scene->materials[collision.materialIndex];
	glm::vec3 f =  material.lightColor;

	float terminateProb = MaxComponent(throughput) * 0.98 + 0.01;
	if (RandomFloat(0.0f, 1.0f) > terminateProb)
		return f;

	Ray scatteredRay;
	glm::vec3 brdfMultiplier = material.Sample(ray, collision, scatteredRay);
	throughput *= brdfMultiplier / terminateProb;

	int32_t indirectSamples = 1;
	glm::vec3 indirectLight = glm::vec3(0);
	for (int32_t i = 0; i < indirectSamples; i++) {
		material.Sample(ray, collision, scatteredRay);
		indirectLight += TraceRay(scatteredRay, maxDepth - 1, throughput);
	}

	glm::vec3 directLight = glm::vec3(0);
	if (material.lightColor == glm::vec3(0) && material.transparency < 0.5f) {
		int32_t lightSamples = 8;
		for (int32_t i = 0; i < lightSamples; i++) {
			for (int32_t i = 0; i < scene->lights.size(); i++) {
				glm::vec3 onLight = scene->lights[i]->GetMesh().RandomInnerPoint();
				glm::vec3 toLight = onLight - collision.position;
				float lightDistance2 = glm::length2(toLight);
				//if (lightDistance2 < 0.001)
				//	continue;
				toLight = glm::normalize(toLight);
				if (glm::dot(collision.normal, toLight) < 0.001)
					continue;
				float lightArea = scene->lights[i]->GetMesh().area;
				//float pdf = (lightArea * fabs(glm::dot(collision.normal, toLight) * fabs(glm::dot(collision.normal, ray.direction)))) / lightDistance2;
				float pdf = (lightArea * fabs(toLight.y)) / lightDistance2;

				Ray lightRay(collision.position, toLight);
				CollisionInfo lightCollision;
				scene->CheckCollision(lightRay, 0.001, 10000.0, lightCollision);
				if (!lightCollision.collided)
					continue;
				const Material& lightMaterial = scene->materials[lightCollision.materialIndex];
				directLight += material.albedo * lightMaterial.lightColor * pdf * (glm::max(0.0f, glm::dot(collision.normal, toLight)) * INV_PI);
			}
		}
		directLight /= (float)lightSamples;
	}

	f += directLight;

	f += (indirectLight / (float)indirectSamples) * brdfMultiplier;

	return f / terminateProb;
}