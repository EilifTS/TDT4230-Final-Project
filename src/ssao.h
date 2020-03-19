#pragma once
#include <string>
#include "utilities/textures.h"
#include <glm/glm.hpp>
#include "utilities/shader.hpp"

#define SSAO_NUM_SAMPLES 16
#define SSAO_RANDOM_SIZE 4

class SSAO
{
public:
	SSAO(int windowWidth, int windowHeight, const std::string& res_path);

	void Render(unsigned int normalDepthID, const glm::mat4& projection, const glm::mat4& invProjection);

	unsigned int GetTexture() { return target.textureIDs[0]; };

private:
	void createSamplePoints();

private:
	Textures::RenderTarget target;
	Textures::RenderTarget middleTarget;
	Gloom::Shader ssaoShader;
	Gloom::Shader blurShader;

	unsigned int squareVAO;
	unsigned int squareIndexCount;

	unsigned int randomTextureID;

	glm::vec2 randomScale;
	float radius;
	glm::vec3 samplePoints[SSAO_NUM_SAMPLES];

	glm::vec2 pixelSize;
};