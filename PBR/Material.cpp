#include "pch.h"
#include "Material.h"


Material::Material(glm::vec3 _albedo, float _roughness, float _metallic, float _transparency, float _refraction)
	: albedo(_albedo), roughness(_roughness), metallic(_metallic), transparency(_transparency), refraction(_refraction), texture(nullptr), ao(0), isLightSource(false), lightColor(glm::vec3(0.0))
{

}

Material::~Material() {}


glm::vec3 Reflect(const glm::vec3& v, const glm::vec3& n) {
	return v - 2 * glm::dot(v, n) * n;
}

glm::vec3 Refract(const glm::vec3& uv, const glm::vec3& n, float etai_over_etat) {
	float cos_theta = fmin(glm::dot(-uv, n), 1.0f);
	glm::vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
	glm::vec3 r_out_parallel = -sqrt(fabs(1.0f - glm::length2(r_out_perp))) * n;
	return r_out_perp + r_out_parallel;
}

float Reflectance(float cosine, float ref_idx) {
	// Use Schlick's approximation for reflectance.
	auto r0 = (1.0f - ref_idx) / (1.0f + ref_idx);
	r0 = r0 * r0;
	return r0 + (1.0f - r0) * pow((1.0f - cosine), 5.0);
}

inline glm::vec3 Material::Evaluate(const CollisionInfo& collision) const {
	glm::vec3 color = albedo;
	if (texture) 
		color *= texture->Sample(collision.uv, collision.position);
	return color / PI;
}

inline constexpr float Material::Pdf() const {
	return 1.0f / (2.0f * PI);
}

glm::vec3 Material::Sample(const Ray& ray, const CollisionInfo& collision, Ray& scatteredRay) const {
	ONB uvw;
	uvw.build_from_w(collision.normal);
	float rougness2 = roughness * roughness;
	float invRoughness2 = 1.0 - rougness2;
	glm::vec3 direction;

	if (transparency > 0.5f) {
		float _refraction = !collision.backFace ? refraction : (1.0 / refraction);

		float cosTheta = fmin(glm::dot(-ray.direction, collision.normal), 1.0f);
		float sinTheta = sqrt(1.0f - cosTheta * cosTheta);

		bool cannotRefract = _refraction * sinTheta > 1.0f;
		if (cannotRefract || Reflectance(cosTheta, _refraction) > RandomFloat()) {
			direction = Reflect(ray.direction, collision.normal);
		}
		else {
			direction = Refract(ray.direction, collision.normal, _refraction);
		}
	}
	else {
		direction = glm::reflect(ray.direction, collision.normal);
		float theta = glm::acos(direction.z) + roughness * RandomFloat(-1.0, 1.0) * PI;
		float phi = atan(direction.y / direction.x) + roughness * RandomFloat(-1.0, 1.0) * PI;
		direction = glm::reflect(glm::vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta)), collision.normal);

		if (glm::dot(direction, collision.normal) < 0) {
			direction = glm::reflect(direction, collision.normal);
		}

		//direction = glm::normalize(Reflect(ray.direction, collision.normal) * invRoughness2 + uvw.local(RandomCosineDirection()) * roughness);
		direction = glm::normalize(uvw.local(RandomCosineDirection()));
	}

	scatteredRay.origin = collision.position;
	scatteredRay.direction = direction;
	float cosTheta = glm::abs(glm::dot(collision.normal, scatteredRay.direction));
	return (cosTheta * Evaluate(collision)) / Pdf();
}