#include "camera.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(const glm::vec3& position, int windowWidth, int windowHeight)
	: position(position),
	rotationX(0.0f), rotationY(0.0f),
	projection(glm::perspective(glm::radians(FIELD_OF_VIEW), float(windowWidth) / float(windowHeight), NEAR_PLANE, FAR_PLANE))
{
	updateView();
}

void Camera::Move(const glm::vec3& deltaPos, float deltaRotX, float deltaRotY)
{
	glm::vec4 deltaPos4 = glm::vec4(deltaPos, 1.0f) * 
		glm::rotate(rotationX, glm::vec3(1, 0, 0)) * 
		glm::rotate(rotationY, glm::vec3(0, 1, 0));
	rotationX += deltaRotX;
	rotationY += deltaRotY;
	position += glm::vec3(deltaPos4.x, deltaPos4.y, deltaPos4.z);
	updateView();
}

const glm::mat4& Camera::View() const
{
	return view;
}
const glm::mat4& Camera::Projection() const
{
	return projection;
}

void Camera::updateView()
{
	view = 
		glm::rotate(rotationX, glm::vec3(1, 0, 0)) *
		glm::rotate(rotationY, glm::vec3(0, 1, 0)) *
		glm::translate(-position);
}