#include "firefly.h"
#include <random>
#include "utilities/mesh.h"
#include "utilities/shapes.h"
#include "utilities/glutils.h"
#include "utilities/textures.h"
#include <glm/gtc/type_ptr.hpp>

namespace
{
	const float cutoff = 5.0f / 256.0f;
	const float la = 1.0f;
	const float lb = 0.7f;
	const float lc = 1.8f;

	float calculateMaxRadius(float maxIntensity)
	{
		float l1 = (la - maxIntensity / cutoff);
		float l2 = lb * lb - 4 * lc * l1;
		return (-lb + std::sqrtf(l2)) / (2 * lc);
	}
}

Fireflies::Fireflies(int windowWidth, int windowHeight, unsigned int count, const std::string& resource_path)
{
	srand(86859302);
	for (int i = 0; i < count; i++)
	{
		glm::vec3 pos = {
			(float)rand() / RAND_MAX,
			0.0f,
			(float)rand() / RAND_MAX,
		};
		pos -= glm::vec3(0.5f, 0.0f, 0.5f);
		pos *= 50.0f;
		pos.y += 0.6f;

		float maxIntensity = 0.5f + (float)rand() / RAND_MAX;
		float maxRadius = calculateMaxRadius(maxIntensity);
		fireflies.push_back({ pos, maxIntensity, maxRadius });
	}
	
	pointLightShader.makeBasicShader(resource_path + "shaders/pointLight.vert", resource_path + "shaders/pointLight.frag");

	Mesh sphere = generateSphere(1.0f, 100, 100);
	sphereVAO = generateBuffer(sphere);
	sphereIndexCount = sphere.indices.size();

	lightTarget = Textures::CreateRenderTarget(windowWidth, windowHeight, 1, false);
}

void Fireflies::Update() {}
void Fireflies::RenderFlies()
{
	
}
void Fireflies::RenderLights(unsigned int posID, unsigned int normalID, unsigned int depthID, const glm::mat4& V, const glm::mat4& P)
{
	glDepthFunc(GL_GREATER);
	glDepthMask(false);
	glFrontFace(GL_CW); 
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	glBindFramebuffer(GL_FRAMEBUFFER, lightTarget.targetID);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthID);


	pointLightShader.activate();
	glBindVertexArray(sphereVAO);
	glBindTextureUnit(0, posID);
	glBindTextureUnit(1, normalID);
	glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(P));

	for (const firefly& f : fireflies)
	{
		glm::vec3 viewPos = V * glm::vec4(f.pos, 1.0f);

		glUniform3fv(0, 1, glm::value_ptr(viewPos));
		glUniform1f(2, f.maxIntensity);
		glUniform1f(3, f.maxRadius);

		glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, nullptr);
	}

	glDisable(GL_BLEND);
	glFrontFace(GL_CCW);
	glDepthMask(true);
	glDepthFunc(GL_LESS);
}