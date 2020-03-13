#pragma once
#include <string>
#include "mesh.h"

namespace OBJLoader
{
	Mesh LoadFromFile(const std::string& path);
}