#include "pch.h"
#include "Camera.h"


Camera::Camera(glm::vec3 _lookFrom, glm::vec3 _lookAt, glm::vec3 _up, float _fov, float _aspect, float _aperture, float _focusDistance, float _near, float _far, float _time0, float _time1)
	: fov(_fov), aspect(_aspect), near(_near), focusDistance(_focusDistance), lensRadius(_aperture / 2.0f), far(_far), speed(10.0f), time0(_time0), time1(_time1)
{
	viewportHeight = 2.0 * tan(glm::radians(fov) / 2);
	viewportWidth = aspect * viewportHeight;

	LookAt(_lookFrom, _lookAt, _up);
}

Camera::Camera(float _aspect, float _fov, float _near, float _far)
	: aspect(_aspect), fov(_fov), near(_near), far(_far), speed(5.0), time0(0), time1(1.0)
{
	float theta = glm::radians(fov);
	float h = tan(theta / 2);
	viewportHeight = 2.0f * h;
	viewportWidth = aspect * viewportHeight;

	RecalculateView();
	RecalculateProjection();
}

Camera::~Camera() {

}

void Camera::LookAt(const glm::vec3& _lookFrom, const glm::vec3& _lookAt, const glm::vec3& _up) {
	transform.SetPosition(_lookFrom);
	lookAt = _lookAt;

	w = glm::normalize(_lookFrom - _lookAt);
	u = glm::normalize(glm::cross(_up, w));
	v = glm::cross(w, u);

	horizontal = focusDistance * viewportWidth * u;
	vertical = focusDistance * viewportHeight * v;
	lbCorner = _lookFrom - horizontal / 2.0f - vertical / 2.0f - focusDistance * w;

	RecalculateView();
	RecalculateProjection();
}

Ray Camera::GetRay(const glm::vec2& coord) const {
	glm::vec3 rd = lensRadius * glm::normalize(RandomVector(-1.0, 1.0));
	glm::vec3 offset = u * rd.x + v * rd.y;

	return Ray(
		transform.GetPosition(),
		glm::normalize(lbCorner + coord.x * horizontal + coord.y * vertical - transform.GetPosition()),
		RandomFloat(time0, time1)
	);


	// matrix variant
	glm::vec4 target = mInverseProjection * glm::vec4(coord.x, coord.y , 1.0, 1.0);
	glm::vec3 direction = glm::normalize(glm::vec3(mInverseView * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0)));
	return Ray(transform.GetPosition() + offset, direction - offset, RandomFloat(time0, time1));
}

void Camera::OnUpdate() {
	if (UserInput::buttonPressed) {
		glm::vec3 forward = -glm::normalize(glm::vec3(w.x, 0, w.z));
		glm::vec3 right = glm::normalize(glm::vec3(u.x, 0, u.z));

		switch ((char)UserInput::key) {
		case GLFW_KEY_W:
			LookAt(transform.GetPosition() + forward, lookAt + forward, v);
			break;
		case GLFW_KEY_S:
			LookAt(transform.GetPosition() - forward, lookAt - forward, v);
			break;
		case GLFW_KEY_A:
			LookAt(transform.GetPosition() - right, lookAt - right, v);
			break;
		case GLFW_KEY_D:
			LookAt(transform.GetPosition() + right, lookAt + right, v);
			break;
		}

		RecalculateView();
	}

	if (UserInput::mouseButton == GLFW_MOUSE_BUTTON_RIGHT && UserInput::mouseAction == GLFW_PRESS) {
		/*
		glm::vec3 rotation = transform.GetUp() * (UserInput::mouseDeltaX / 360.0f) + transform.GetRight() * (-UserInput::mouseDeltaY / 360.0f);
		transform.Rotate(rotation);
		glm::quat rot = rotation;
		LookAt(transform.GetPosition(), lookAt * rot, v);

		RecalculateView();

		glm::vec3 dir = GetRay(glm::vec2(0.0f, 0.0f)).direction;
		std::cout << dir.x << " " << dir.y << " " << dir.z << "\n";
		*/
	}
}

const glm::mat4& Camera::GetViewMatrix() const {
	return mView;
}

const glm::mat4& Camera::GetInverseViewMatrix() const {
	return mInverseView;
}

const glm::mat4& Camera::GetProjectionMatrix() const {
	return mProjection;
}

const glm::mat4& Camera::GetInverseProjectionMatrix() const {
	return mInverseProjection;
}

void Camera::RecalculateView() {
	mView = glm::lookAt(transform.GetPosition(), lookAt, transform.GetUp());
	mInverseView = glm::inverse(mView);
}

void Camera::RecalculateProjection() {
	mProjection = glm::perspective(glm::radians(fov), aspect, 0.1f, 10000.0f);
	mInverseProjection = glm::inverse(mProjection);
}