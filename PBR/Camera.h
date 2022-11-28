#include "pch.h"
#include "Transform.h"
#include "Ray.h"
#include "UserInput.h"
#include "Time.h"
#include "Random.h"

class Camera
{
public:
	Transform transform;

	Camera(glm::vec3 _lookFrom, glm::vec3 _lookAt, glm::vec3 _up, float _fov, float _aspect, float _aperture, float _focusDistance, float _near = 0.0001f, float _far = 10000.0, float _time0 = 0.0f, float _time1 = 1.0f);
	Camera(float aspect = (16.0/9.0), float fov = 45.0, float near = 0.001, float far = 1000.0);
	~Camera();

	void LookAt(const glm::vec3& _lookFrom, const glm::vec3& _lookAt, const glm::vec3& _up);
	Ray GetRay(const glm::vec2& coord) const;

	const glm::mat4& GetViewMatrix() const;
	const glm::mat4& GetInverseViewMatrix() const;
	const glm::mat4& GetProjectionMatrix() const;
	const glm::mat4& GetInverseProjectionMatrix() const;

	void OnUpdate();
private:
	float fov;
	float aspect;
	float near;
	float far;
	float lensRadius;
	float focusDistance;

	glm::vec3 lookAt;

	float speed;

	float viewportWidth;
	float viewportHeight;

	glm::mat4 mView;
	glm::mat4 mInverseView;
	glm::mat4 mProjection;
	glm::mat4 mInverseProjection;

	glm::vec3 w;
	glm::vec3 u;
	glm::vec3 v;
	glm::vec3 horizontal;
	glm::vec3 vertical;
	glm::vec3 lbCorner;

	float time0;
	float time1;

	void RecalculateView();
	void RecalculateProjection();
};