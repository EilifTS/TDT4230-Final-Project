#include "firefly.h"
#include <random>
#include "utilities/mesh.h"
#include "utilities/shapes.h"
#include "utilities/glutils.h"
#include "utilities/textures.h"
#include <glm/gtc/type_ptr.hpp>

namespace
{
	const float cutoff = 3.0f / 256.0f;
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

Fireflies::Fireflies(SceneNode* root, int windowWidth, int windowHeight, unsigned int count, const std::string& resource_path)
{
	srand(86859302);

	unsigned int fireflyTextID = Textures::LoadPNG(resource_path + "textures/firefly.png");
	Mesh sphere = generateSphere(1.0f, 50, 50);
	sphereVAO = generateBuffer(sphere);
	sphereIndexCount = sphere.indices.size();

	for (int i = 0; i < count; i++)
	{
		float rndAngle = 2.0f * 3.141592f * ((float)rand() / RAND_MAX);
		float rndDist = 1.0f + 1.0f * ((float)rand() / RAND_MAX);
		glm::vec3 pos = glm::vec3(cosf(rndAngle), 0, sinf(rndAngle)) * rndDist;

		float phase = (float)rand() / RAND_MAX;
		float maxIntensity = 0.5f;// +((float)rand() / RAND_MAX);
		float maxRadius = calculateMaxRadius(maxIntensity);

		fireflies.push_back({ pos, phase, maxIntensity, maxRadius });
	}
	
	pointLightShader.makeBasicShader(resource_path + "shaders/pointLight.vert", resource_path + "shaders/pointLight.frag");
	fireflyShader.makeBasicShader(resource_path + "shaders/firefly.vert", resource_path + "shaders/firefly.frag");
	fireflyTarget = Textures::CreateRenderTarget(windowWidth, windowHeight, { {GL_RGBA16F, GL_FLOAT} }, false);
	lightTarget = Textures::CreateRenderTarget(windowWidth, windowHeight, { {GL_RGBA16F, GL_FLOAT} }, false);
}

void Fireflies::Update(double time)
{
	for (auto& f : fireflies)
	{
		float distPhase = fmodf(f.phase * 451.231, 1.0f);
		//distPhase *= distPhase;
		float dist = 0.4f + 20.0f * distPhase;

		float way = f.phase * 2.0f - 1.0f;
		way = way < 0 ? -1 : 1;
		way = 1;
		float heightPhase = fmodf(time * 0.0231f + f.phase, 1.0f);
		float anglePhase = fmodf(f.phase * 577.3111f + way*time*0.0724 / dist, 1.0f);
		float height = 0.5f + 3.4f * 0.5f* (1.0f + sinf(2*3.141592f * heightPhase)) / (1.0f+distPhase);
		float angle = 2.0f * 3.141592f * anglePhase;
		f.pos = glm::vec3(cosf(angle), 0, sinf(angle)) * dist;
		f.pos.y = height;
	}
}
void Fireflies::RenderFlies(unsigned int depthID, const glm::mat4& V, const glm::mat4& P) // depthID, cameraMatrix
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glDepthMask(false);

	glBindFramebuffer(GL_FRAMEBUFFER, fireflyTarget.targetID);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthID);

	fireflyShader.activate();
	glBindVertexArray(sphereVAO);
	glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(P));

	for (const firefly& f : fireflies)
	{
		glm::vec3 viewPos = V * glm::vec4(f.pos, 1.0f);
		glUniform3fv(0, 1, glm::value_ptr(viewPos));

		glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, nullptr);
	}

	glDepthMask(true);
	glDisable(GL_BLEND);
}

void Fireflies::RenderLights(unsigned int normalDepthID, unsigned int depthID, const glm::mat4& V, const glm::mat4& P)
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
	glBindTextureUnit(0, normalDepthID);
	glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(P));

	for (const firefly& f : fireflies)
	{
		glm::vec3 viewPos = V * glm::vec4(f.pos, 1.0f);

		glUniform3fv(0, 1, glm::value_ptr(viewPos));
		glUniform1f(2, fmod(f.phase * 73.5764f, 1.0f));
		glUniform1f(3, f.maxIntensity);
		glUniform1f(4, f.maxRadius);

		glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, nullptr);
	}

	glDisable(GL_BLEND);
	glFrontFace(GL_CCW);
	glDepthMask(true);
	glDepthFunc(GL_LESS);
}