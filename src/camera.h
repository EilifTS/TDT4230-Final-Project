#pragma once
#include <glm/glm.hpp>

#define FIELD_OF_VIEW 60.0f
#define NEAR_PLANE 0.1f
#define FAR_PLANE 100.0f

class Camera
{
public:
	Camera(const glm::vec3& position, int windowWidth, int windowHeight);
	
	void Move(const glm::vec3& deltaPos, float deltaRotX, float deltaRotY);

	const glm::mat4& Projection() const;
	const glm::mat4& InvProjection() const { return invProjection; };
	const glm::mat4& View() const;

private:
	void updateView();

private:
	glm::vec3 position;
	float rotationX, rotationY;
	glm::mat4 projection;
	glm::mat4 invProjection;
	glm::mat4 view;

};