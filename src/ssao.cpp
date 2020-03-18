#include "ssao.h"
#include <random>
#include "utilities/shapes.h"
#include "utilities/glutils.h"
#include <glm/gtc/type_ptr.hpp>

namespace
{
	float randomNormalDist()
	{
		float out = 0.0f;
		for(int i = 0; i < 4; i++)
		{
			float r = (float)rand() / RAND_MAX;
			r *= 2.0f;
			r -= 1.0f;
			out += r;
		}
		return out;
	}

	unsigned int createRandomTexture()
	{
		std::vector<glm::vec4> data((long long)SSAO_RANDOM_SIZE * SSAO_RANDOM_SIZE);

		for (unsigned int y = 0; y < SSAO_RANDOM_SIZE; y++)
		{
			for (unsigned int x = 0; x < SSAO_RANDOM_SIZE; x++)
			{
				glm::vec4 new_vec(randomNormalDist(), randomNormalDist(), 0.0f, 0.0f);
				new_vec = glm::normalize(new_vec);
				data[(long long)y * SSAO_RANDOM_SIZE + x] = new_vec;
			}
		}

		return Textures::CreateTextureFromData(SSAO_RANDOM_SIZE, SSAO_RANDOM_SIZE, data);
	}

	
}

SSAO::SSAO(int windowWidth, int windowHeight, const std::string& res_path)
{
	srand(1001110101);
	target = Textures::CreateRenderTarget(windowWidth, windowHeight, { {GL_RGBA, GL_FLOAT} }, false);
	ssaoShader.makeBasicShader(res_path + "shaders/ssao.vert", res_path + "shaders/ssao.frag");

	Mesh square = generateSquare();
	squareVAO = generateBuffer(square);
	squareIndexCount = square.indices.size();

	randomTextureID = createRandomTexture();
	randomScale.x = (float)windowWidth / (float)SSAO_RANDOM_SIZE;
	randomScale.y = (float)windowHeight / (float)SSAO_RANDOM_SIZE;
	radius = 1.0f;

	createSamplePoints();

}

void SSAO::Render(unsigned int normalDepthID, const glm::mat4& projection, const glm::mat4& invProjection)
{
	glDisable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, target.targetID);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	ssaoShader.activate();
	glBindTextureUnit(0, normalDepthID);
	glBindTextureUnit(1, randomTextureID);

	glBindVertexArray(squareVAO);
	glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(invProjection));
	glUniform2fv(2, 1, glm::value_ptr(randomScale));
	glUniform1f(3, radius);
	glUniform3fv(4, SSAO_NUM_SAMPLES, glm::value_ptr(samplePoints[0]));

	glDrawElements(GL_TRIANGLES, squareIndexCount, GL_UNSIGNED_INT, nullptr);

	glEnable(GL_DEPTH_TEST);
}

void SSAO::createSamplePoints()
{
	for (unsigned int i = 0; i < SSAO_NUM_SAMPLES; i++)
	{
		glm::vec3 new_point(randomNormalDist(), randomNormalDist(), 0.1 + 0.9*abs(randomNormalDist()));
		new_point = glm::normalize(new_point);
		float scale = ((float)i / SSAO_NUM_SAMPLES);
		scale *= scale;
		scale = 0.1f + scale * 0.9f;
		new_point *= scale;
		samplePoints[i] = new_point;
	}
}