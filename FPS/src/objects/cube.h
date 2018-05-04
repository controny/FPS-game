#ifndef CUBE_H
#define CUBE_H
#include "cube.h"
class Cube : public Mesh {
public:
	//
	Cube(glm::vec3 center, float length, const unsigned int &diffuse, const unsigned int &specular) :
		Mesh(getCube(center, length, diffuse, specular)) {
		std::cout << vertices.size();
		for (int i = 0; i < 36; ++i)
			std::cout << indices[i] << " ";
	}
	Mesh getCube(glm::vec3 center, float length, const unsigned int &diffuse, const unsigned int &specular) {
		float coords[] = {
			-0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f,  0.5f, -0.5f,
			0.5f,  0.5f, -0.5f,
			-0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f, -0.5f,  0.5f,
			0.5f, -0.5f,  0.5f,
			0.5f,  0.5f,  0.5f,
			0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,

			-0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,

			0.5f,  0.5f,  0.5f,
			0.5f,  0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f, -0.5f,  0.5f,
			0.5f,  0.5f,  0.5f,

			-0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f, -0.5f,  0.5f,
			0.5f, -0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f,  0.5f, -0.5f,
			0.5f,  0.5f, -0.5f,
			0.5f,  0.5f,  0.5f,
			0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f, -0.5f,
		};
		for (int i = 0; i < 36 * 3; ++i) {
			coords[i] = coords[i] * length;
			if ((i + 1) % 3 == 1) coords[i] += center.x;
			else if ((i + 1) % 3 == 2) coords[i] += center.y;
			else coords[i] += center.z;
		}
		float normals[] = {
			0.0f,  0.0f, -1.0f,
			0.0f,  0.0f, -1.0f,
			0.0f,  0.0f, -1.0f,
			0.0f,  0.0f, -1.0f,
			0.0f,  0.0f, -1.0f,
			0.0f,  0.0f, -1.0f,

			0.0f,  0.0f, 1.0f,
			0.0f,  0.0f, 1.0f,
			0.0f,  0.0f, 1.0f,
			0.0f,  0.0f, 1.0f,
			0.0f,  0.0f, 1.0f,
			0.0f,  0.0f, 1.0f,

			-1.0f,  0.0f,  0.0f,
			-1.0f,  0.0f,  0.0f,
			-1.0f,  0.0f,  0.0f,
			-1.0f,  0.0f,  0.0f,
			-1.0f,  0.0f,  0.0f,
			-1.0f,  0.0f,  0.0f,

			1.0f,  0.0f,  0.0f,
			1.0f,  0.0f,  0.0f,
			1.0f,  0.0f,  0.0f,
			1.0f,  0.0f,  0.0f,
			1.0f,  0.0f,  0.0f,
			1.0f,  0.0f,  0.0f,

			0.0f, -1.0f,  0.0f,
			0.0f, -1.0f,  0.0f,
			0.0f, -1.0f,  0.0f,
			0.0f, -1.0f,  0.0f,
			0.0f, -1.0f,  0.0f,
			0.0f, -1.0f,  0.0f,

			0.0f,  1.0f,  0.0f,
			0.0f,  1.0f,  0.0f,
			0.0f,  1.0f,  0.0f,
			0.0f,  1.0f,  0.0f,
			0.0f,  1.0f,  0.0f,
			0.0f,  1.0f,  0.0f
		};
		std::vector<Vertex>vertices;
		std::vector<unsigned int>indices;
		std::vector<Texture>textures;
		if (diffuse > 0)
			textures.push_back(Texture(diffuse, "diffuse"));
		if (specular > 0)
			textures.push_back(Texture(specular, "specular"));
		float tex_coords[] = {
			// Back face
			0.0f, 0.0f, // Bottom-left
			1.0f, 1.0f, // top-right
			1.0f, 0.0f, // bottom-right         
			1.0f, 1.0f,  // top-right
			0.0f, 0.0f,  // bottom-left
			0.0f, 1.0f,
			// Front face
			0.0f, 0.0f, // bottom-left
			1.0f, 0.0f,  // bottom-right
			1.0f, 1.0f,  // top-right
			1.0f, 1.0f, // top-right
			0.0f, 1.0f,  // top-left
			0.0f, 0.0f,
			// Left face
			1.0f, 0.0f, // top-right
			1.0f, 1.0f, // top-left
			0.0f, 1.0f,  // bottom-left
			0.0f, 1.0f, // bottom-left
			0.0f, 0.0f,  // bottom-right
			1.0f, 0.0f,
			// Right face
			1.0f, 0.0f, // top-left
			0.0f, 1.0f, // bottom-right
			1.0f, 1.0f, // top-right         
			0.0f, 1.0f,  // bottom-right
			1.0f, 0.0f,  // top-left
			0.0f, 0.0f,
			// Bottom face
			0.0f, 1.0f, // top-right
			1.0f, 1.0f, // top-left
			1.0f, 0.0f,// bottom-left
			1.0f, 0.0f, // bottom-left
			0.0f, 0.0f, // bottom-right
			0.0f, 1.0f,
			// Top face
			0.0f, 1.0f,// top-left
			1.0f, 0.0f, // bottom-right
			1.0f, 1.0f, // top-right     
			1.0f, 0.0f, // bottom-right
			0.0f, 1.0f,// top-left
			0.0f, 0.0f
		};
		for (int i = 0; i < 36; i++) {
			vertices.push_back(Vertex(glm::vec3(coords[i * 3], coords[i * 3 + 1], coords[i * 3 + 2]),
				glm::vec3(normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]), glm::vec2(tex_coords[i * 2], tex_coords[i * 2 + 1])));
		}
		for (int i = 0; i < 36; ++i)
			indices.push_back(i);
		return Mesh(vertices, indices, textures);
	}
};
#endif // !CUBE_H
