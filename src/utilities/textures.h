#pragma once
#include <string>
#include <vector>

namespace Textures
{
	unsigned int LoadPNG(const std::string& file_path);

	struct RenderTarget
	{
		unsigned int targetID;
		unsigned int depthID;
		std::vector<unsigned int> textureIDs;
	};
	RenderTarget CreateRenderTarget(int width, int height, int textureCount, bool depth);
}