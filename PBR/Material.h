#pragma once
#include "pch.h"
#include "CollisionInfo.h"
#include "Ray.h"
#include "Random.h"
#include "Texture.h"
#include "Spectrum.h"

struct Material {
	glm::vec3 albedo;
	std::shared_ptr<Texture> texture;

	float roughness;
	float metallic;
	float transparency;
	float ao;
	float refraction;

	bool isLightSource;
	glm::vec3 lightColor;

	Material(glm::vec3 _albedo = glm::vec3(1.0), float _roughness = 1.0, float _metallic = 0.0, float _transparency = 0.0, float _refraction = 1.0);
	~Material();

	inline glm::vec3 Evaluate(const CollisionInfo& collision) const;
	inline constexpr float Pdf() const;
	glm::vec3 Sample(const Ray& ray, const CollisionInfo& collision, Ray& scatteredRay) const;
};

float Reflectance(float cosine, float ref_idx);
glm::vec3 Reflect(const glm::vec3& v, const glm::vec3& n);
glm::vec3 Refract(const glm::vec3& uv, const glm::vec3& n, float etai_over_etat);