#pragma once
#include "pch.h"

class Transform {
	glm::vec3 position;
	glm::vec3 scale;
	glm::quat rotation;

	glm::vec3 forward;
	glm::vec3 up;
	glm::vec3 right;

	glm::mat4 mTranslate;
	glm::mat4 mRotate;
	glm::mat4 mScale;
	glm::mat4 mTransform;
	glm::mat4 mInverseTransform;

	void RecalculateTransformMatrix();
	void RecalculateRotations();
public:

	Transform(glm::vec3 _position = glm::vec3(0.0), glm::vec3 _rotation = glm::vec3(0.0), glm::vec3 _scale = glm::vec3(1.0));
	~Transform();

	void SetPosition(glm::vec3 _position);
	void SetRotation(glm::vec3 _rotation);
	void SetScale(glm::vec3 _scale);

	void Translate(glm::vec3 _offset);
	void Rotate(glm::vec3 _rotations);
	void RotateX(float angle);
	void RotateY(float angle);
	void RotateZ(float angle);
	void Scale(glm::vec3 _scale);

	const glm::vec3& GetPosition() const;
	const glm::quat& GetRotation() const;
	const glm::vec3& GetScale() const;

	const glm::vec3& GetForward() const;
	const glm::vec3& GetUp() const;
	const glm::vec3& GetRight() const;

	const glm::mat4& GetTranslationMatrix() const;
	const glm::mat4& GetRotationMatrix() const;
	const glm::mat4& GetScaleMatrix() const;
	const glm::mat4& GetTransformMatrix() const;
	const glm::mat4& GetInverseTransformMatrix() const;
};

