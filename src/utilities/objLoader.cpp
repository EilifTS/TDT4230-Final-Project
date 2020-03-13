#include "objLoader.h"
#include <fstream>
#include <sstream>
#include <assert.h>
#include <algorithm>

Mesh OBJLoader::LoadFromFile(const std::string& path)
{
	Mesh out;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uv;
	std::vector<glm::vec3> temp_normals;
	int index = 0;
	std::ifstream file(path);
	assert(!file.fail());

	std::string line_type;

	while (file >> line_type)
	{
		
		if (line_type == "v") // Read vertex
		{
			glm::vec3 v;
			file >> v.x >> v.y >> v.z;
			temp_vertices.push_back(v);
		}
		else if (line_type == "vt") // Read texture coordinate
		{
			glm::vec2 vt;
			file >> vt.x >> vt.y;
			temp_uv.push_back(vt);
		}
		else if(line_type == "vn") // Read normal
		{
			glm::vec3 n;
			file >> n.x >> n.y >> n.z;
			temp_normals.push_back(n);
		}
		else if (line_type == "f") // Read face
		{
			// Skipping indexing :)
			unsigned int face_vertex_count = 0;
			std::string face_string;

			for (int i = 0; i < 3; i++) // Read in the three vertices of the face
			{
				file >> face_string;
			
				std::replace(face_string.begin(), face_string.end(), '/', ' ');
				int v_index, vt_index, n_index;
				std::istringstream ss_index(face_string);

				
				ss_index >> v_index;
				ss_index >> vt_index;
				ss_index >> n_index;

				out.vertices.push_back(temp_vertices[v_index - 1]);
				out.textureCoordinates.push_back(temp_uv[vt_index - 1]);
				out.normals.push_back(temp_normals[n_index - 1]);
				out.indices.push_back(index++);
			}
		}
	}
	return out;
}