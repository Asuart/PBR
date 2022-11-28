#include "pch.h"
#include "Transform.h"

Transform::Transform(glm::vec3 _position, glm::vec3 _rotation, glm::vec3 _scale)
	: position(_position), rotation(_rotation), scale(_scale) 
{
	forward = glm::vec3(1, 0, 0);
	up = glm::vec3(0, 1, 0);
	right = glm::vec3(0, 0, 1);

	mTranslate = glm::mat4(1.0);
	mRotate = glm::mat4(1.0);
	mScale = glm::mat4(1.0);
	mTransform = glm::mat4(1.0);

	SetPosition(_position);
	SetRotation(_rotation);
	SetScale(_scale);
}

Transform::~Transform() {

}

void Transform::SetPosition(glm::vec3 _position) {
	position = _position;
	mTranslate = glm::translate(glm::mat4(1.0), position);
	RecalculateTransformMatrix();
}

void Transform::SetRotation(glm::vec3 _rotation) {
	rotation = glm::quat(_rotation);
	mRotate = glm::toMat4(rotation);
	forward = mRotate * glm::vec4(0, 0, 1, 1);
	up = mRotate * glm::vec4(0, 1, 0, 1);
	right = mRotate * glm::vec4(-1, 0, 0, 1);
}

void Transform::SetScale(glm::vec3 _scale) {
	scale = _scale;
	mScale = glm::scale(glm::mat4(1.0), scale);
	RecalculateTransformMatrix();
}

void Transform::Translate(glm::vec3 _offset) {
	position += _offset;
	mTranslate = glm::translate(mTranslate, position);
	RecalculateTransformMatrix();
}

void Transform::Rotate(glm::vec3 _rotations) {
	rotation = glm::quat(glm::quat(_rotations) * rotation);
	RecalculateRotations();
}

void Transform::RotateX(float angle) {
	rotation = glm::quat(glm::quat(glm::vec3(angle, 0, 0)) * rotation);
	RecalculateRotations();
}

void Transform::RotateY(float angle) {
	rotation = glm::quat(glm::quat(glm::vec3(0, angle, 0)) * rotation);
	RecalculateRotations();
}

void Transform::RotateZ(float angle) {
	rotation = glm::quat(glm::quat(glm::vec3(0, 0, angle)) * rotation);
	RecalculateRotations();
}

void Transform::Scale(glm::vec3 _scale) {
	scale += _scale;
	mScale = glm::scale(mScale, scale);
	RecalculateTransformMatrix();
}

const glm::vec3& Transform::GetPosition() const {
	return position;
}

const glm::quat& Transform::GetRotation() const {
	return rotation;
}

const glm::vec3& Transform::GetScale() const {
	return scale;
}

const glm::vec3& Transform::GetForward() const {
	return forward;
}

const glm::vec3& Transform::GetUp() const {
	return up;
}

const glm::vec3& Transform::GetRight() const {
	return right;
}

const glm::mat4& Transform::GetTranslationMatrix() const {
	return mTranslate;
}

const glm::mat4& Transform::GetRotationMatrix() const {
	return mRotate;
}

const glm::mat4& Transform::GetScaleMatrix() const {
	return mScale;
}

const glm::mat4& Transform::GetTransformMatrix() const {
	return mTransform;
}

const glm::mat4& Transform::GetInverseTransformMatrix() const {
	return mInverseTransform;
}

void Transform::RecalculateTransformMatrix() {
	mTransform = mTranslate * mRotate * mScale;
	mInverseTransform = glm::inverse(mTransform);
}

void Transform::RecalculateRotations() {
	mRotate = glm::toMat4(rotation);
	forward = mRotate * glm::vec4(0, 0, 1, 1);
	up = mRotate * glm::vec4(0, 1, 0, 1);
	right = mRotate * glm::vec4(1, 0, 0, 1);
	RecalculateTransformMatrix();
}