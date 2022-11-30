#include "pch.h"
#include "Material.h"


Material::Material(glm::vec3 _albedo, float _roughness, float _metallic, float _transparency, float _refraction)
	: albedo(_albedo), roughness(_roughness), metallic(_metallic), transparency(_transparency), refraction(_refraction), texture(nullptr), ao(0), isLightSource(false), lightColor(glm::vec3(0.0))
{

}

Material::~Material() {

}

void TestVectorIsUnit(const glm::vec3& v) {
	float length = glm::length(v);
	if (length  > 1.001f || length < 0.999f) std::cout << length << "\n";
}

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


bool Material::Scatter(const Ray& ray, const CollisionInfo& collision, glm::vec3& attenuation, Ray& scattered, float& outPdf) const {
	ONB uvw;
	uvw.build_from_w(collision.normal);
	glm::vec3 scatterDirection = uvw.local(RandomCosineDirection());
	scattered = Ray(collision.position, scatterDirection, ray.time);
	attenuation = albedo;
	outPdf = glm::dot(uvw.w(), scattered.direction) / PI;
	return true;

	// metal
	/*
	glm::vec3 reflected = glm::normalize(reflect(ray.direction, collision.normal) + RandomInUnitSphere() * roughness);
    scattered = Ray(collision.position, reflected, ray.time);
    attenuation = albedo;
    return (glm::dot(scattered.direction, collision.normal) > 0);
	*/

	// glass
	/*
	attenuation = albedo;
    float refraction_ratio = collision.backFace ? refraction : (1.0 / refraction);
    double cos_theta = fmin(glm::dot(-ray.direction, collision.normal), 1.0f);
    double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

    bool cannot_refract = refraction_ratio * sin_theta > 1.0;
    glm::vec3 direction;
    if (cannot_refract || Reflectance(cos_theta, refraction_ratio) > RandomFloat())
        direction = Reflect(ray.direction, collision.normal);
    else
        direction = Refract(ray.direction, collision.normal, refraction_ratio);

    scattered = Ray(collision.position, direction, ray.time);

    return true;
	*/

	// test combination
	/*
	attenuation = albedo * (1.0f - metallic);
	scattered = Ray(collision.position, reflect(ray.direction, collision.normal) + RandomInHemisphere(collision.normal) * roughness * (1.0f - metallic * 0.5f), ray.time);
	return (glm::dot(scattered.direction, collision.normal) != 0);

	bool absorbed = false;
	if (transparency > 0) {
		float refraction = collision.backFace ? refraction : (1.0 / refraction);
		float cosTheta = fmin(glm::dot(-ray.direction, collision.normal), 1.0f);
		float sinTheta = sqrt(1.0f - cosTheta * cosTheta);

		bool cannotRefract = refraction * sinTheta > 1.0f;
		if (cannotRefract || Reflectance(cosTheta, refraction) > RandomFloat()) {
			scattered.direction = reflect(ray.direction, collision.normal) + RandomInHemisphere(collision.normal) * roughness;
		}
		else {
			scattered.direction = refract(ray.direction, collision.normal, refraction);
		}
	}
	else {
		scattered.direction = glm::reflect(ray.direction, collision.normal) + RandomInHemisphere(collision.normal) * roughness;
	}
	*/
}

bool Material::Emitted(const glm::vec2& uv, const glm::vec3& p, glm::vec3& outLightColor) const {
	outLightColor = lightColor;
	return isLightSource;
}

float Material::ScatteringPdf(const Ray& ray, const CollisionInfo& collision, const Ray& scattered) const {
	float cosine = glm::dot(collision.normal, glm::normalize(scattered.direction));
	return cosine < 0 ? 0 : cosine / glm::pi<float>();
}

inline constexpr glm::vec3 Material::Evaluate() const {
	return albedo / PI;
}

inline constexpr float Material::Pdf() const {
	return 1.0f / (2.0f * PI);
}

glm::vec3 Material::Sample(const Ray& ray, const CollisionInfo& collision, Ray& scatteredRay) const {
	ONB uvw;
	uvw.build_from_w(collision.normal);
	glm::vec3 direction = glm::normalize(uvw.local(RandomCosineDirection()));
	scatteredRay.origin = collision.position;
	scatteredRay.direction = direction;
	float cosTheta = glm::abs(glm::dot(collision.normal, scatteredRay.direction));
	return (cosTheta * Evaluate()) / Pdf();
}