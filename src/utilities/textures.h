#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>

namespace Textures
{
	unsigned int LoadPNG(const std::string& file_path);
	
	unsigned int CreateTextureFromData(int width, int height, const std::vector<glm::vec4>& data);

	struct RenderTarget
	{
		unsigned int targetID;
		unsigned int depthID;
		std::vector<unsigned int> textureIDs;
	};
	RenderTarget CreateRenderTarget(int width, int height, const std::vector<std::pair<unsigned int, unsigned int>>& texture_formats, bool depth);

	// G-Buffer
	// 3x8bit Color, 8bit emission
	// 2x16bit normal, 16bit depth, 16bit diffuse
}