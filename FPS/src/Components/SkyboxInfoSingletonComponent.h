#pragma once
#include <ECS.h>
#include <glm/gtc/type_ptr.hpp>
#include <Components/ObjectComponent.h>
#include <vector>

using namespace ECS;

struct SkyboxInfoSingletonComponent {
	std::vector<Vertex> vertices;
	std::vector<Texture> textures;
	std::vector<unsigned int> indices;
	//SkyMesh mesh;
	vector<Mesh> meshes;
	

	SkyboxInfoSingletonComponent(std::vector<Vertex> _vertices, std::vector<unsigned int> _indices, std::vector<Texture> _textures) {
		vertices = _vertices;
		indices = _indices;
		textures = _textures;
		Mesh mesh = Mesh(vertices, indices, textures);
		meshes.push_back(mesh);
	}
};
