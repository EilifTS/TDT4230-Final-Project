#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "utilities/shader.hpp"
#include "utilities/textures.h"

class Fireflies
{
public:
	Fireflies(int windowWidth, int windowHeight, unsigned int count, const std::string& resource_path);

	void Update(double time);
	void RenderFlies();
	void RenderLights(unsigned int normalDepthID, unsigned int depthID, const glm::mat4& V, const glm::mat4& P);

	unsigned int GetLightTexture() { return lightTarget.textureIDs[0]; }

private:
	struct firefly
	{
		glm::vec3 pos;
		float phase;
		float maxIntensity;
		float maxRadius;
	};

	std::vector<firefly> fireflies;
	Gloom::Shader pointLightShader;
	unsigned int sphereVAO;
	unsigned int sphereIndexCount;
	Textures::RenderTarget lightTarget;

};