#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Components/ObjectComponent.h>

using namespace std;

unsigned int Load(const char * path)
{
	if (path == NULL) {
		std::cout << "Path error" << std::endl;
		return 0;
	}
	GLuint texture;
	int width, height, nrChannels;
	unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
	if (data) {
		GLenum format;
		switch (nrChannels)
		{
		case 1:
			format = GL_RED;
			break;
		case 2:
			format = GL_RG;
			break;
		case 3:
			format = GL_RGB;
			break;
		case 4:
			format = GL_RGBA;
			break;
		default:
			format = GL_RGB;
			break;
		}
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
		std::cout << path << std::endl;
		return 0;
	}
	return texture;
}

struct Resource {

	struct TextureResource {
		Texture container;
		Texture container_specular;
		// textures for ground
		Texture ground;
		Texture ground_specular;

		void init() {
			container = Texture(Load("resources/textures/container2.png"), "texture_diffuse");
			container_specular = Texture(Load("resources/textures/container2_specular.png"), "texture_specular");

			ground = Texture(Load("resources/textures/woodDiffuse.jpg"), "texture_diffuse");
			ground_specular = Texture(Load("resources/textures/woodSpecular.jpg"), "texture_specular");
		}
	};

	

	// 生成 cube 所需要的 meshcomponent 中需要的数据
	struct CubeResource {
		
		TextureResource textureResource;
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;

		void init(glm::vec3 center = glm::vec3(), float length = 1) {
			textureResource.init();

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
  
			textures.push_back(textureResource.container);
			textures.push_back(textureResource.container_specular);

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
		}
	};

	struct GroundResource {

		TextureResource textureResource;
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;

		void init(const int &length) {
			textureResource.init();

			glm::vec3 LB = glm::vec3(-length, 0, length),
				RB = glm::vec3(length, 0, length),
				LT = glm::vec3(-length, 0, -length),
				RT = glm::vec3(length, 0, -length);
			glm::vec3 normal = glm::normalize(glm::cross(RB - LB, RT - LB));
			vertices.push_back(Vertex(LB, normal, glm::vec2(0, 0)));
			vertices.push_back(Vertex(RB, normal, glm::vec2(length, 0)));
			vertices.push_back(Vertex(LT, normal, glm::vec2(0, length)));
			vertices.push_back(Vertex(RT, normal, glm::vec2(length, length)));
			indices.push_back(0);
			indices.push_back(1);
			indices.push_back(2);
			indices.push_back(1);
			indices.push_back(3);
			indices.push_back(2);
			// load textures
			textures.push_back(textureResource.ground);
			textures.push_back(textureResource.ground_specular);
		}
	};

	Resource() {
		init();
	}

	void init() {
		
	}

	//CubeResource cubeResource;
};




