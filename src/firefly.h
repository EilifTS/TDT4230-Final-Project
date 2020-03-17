#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "utilities/shader.hpp"
#include "utilities/textures.h"
#include "sceneGraph.hpp"

class Fireflies
{
public:
	Fireflies(SceneNode* root, int windowWidth, int windowHeight, unsigned int count, const std::string& resource_path);

	void Update(double time);
	void RenderFlies(unsigned int depthID, const glm::mat4& V, const glm::mat4& P);
	void RenderLights(unsigned int normalDepthID, unsigned int depthID, const glm::mat4& V, const glm::mat4& P);

	unsigned int GetFireflyTexture() { return fireflyTarget.textureIDs[0]; }
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
	Gloom::Shader fireflyShader;
	unsigned int sphereVAO;
	unsigned int sphereIndexCount;
	Textures::RenderTarget fireflyTarget;
	Textures::RenderTarget lightTarget;

};