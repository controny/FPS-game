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

unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

struct Resource {

	struct TextureResource {
		Texture container_diffuse;
		Texture container_specular;

		// textures for ground
		Texture ground_diffuse;
		Texture ground_specular;

		void init() {
			container_diffuse = Texture(Load("resources/textures/container2.png"), "texture_diffuse");
			container_specular = Texture(Load("resources/textures/container2_specular.png"), "texture_specular");

			ground_diffuse = Texture(Load("resources/textures/wood.png"), "texture_diffuse");
			ground_specular = Texture(Load("resources/textures/wood_specular.jpg"), "texture_specular");
		}
	};

	struct SkyTextureResource {
		Texture cubemapTexture;
		vector<std::string> faces
		{
			"resources/textures/skybox/right.jpg",
			"resources/textures/skybox/left.jpg",
			"resources/textures/skybox/top.jpg",
			"resources/textures/skybox/bottom.jpg",
			"resources/textures/skybox/back.jpg",
			"resources/textures/skybox/front.jpg"
		};
		void init() {
			cubemapTexture = Texture(loadCubemap(faces), "texture_diffuse");
		}
	};
	

	// 生成 cube 所需要的 meshcomponent 中需要的数据
	struct CubeResource {
		
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;

		void init(glm::vec3 center, float length, float width, float height,
			Texture diffuse_texture, Texture specular_texture) {

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
				if ((i + 1) % 3 == 1) coords[i] = coords[i] * length + center.x;
				else if ((i + 1) % 3 == 2) coords[i] = coords[i] * height + center.y;
				else coords[i] = coords[i] * width + center.z;
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
  
			textures.push_back(diffuse_texture);
			textures.push_back(specular_texture);

			float tex_coords[] = {
				0.0f, 0.0f,
				1.0f, 0.0f,
				1.0f, 1.0f,
				1.0f, 1.0f,
				0.0f, 1.0f,
				0.0f, 0.0f,

				0.0f, 0.0f,
				1.0f, 0.0f,
				1.0f, 1.0f,
				1.0f, 1.0f,
				0.0f, 1.0f,
				0.0f, 0.0f,

				1.0f, 0.0f,
				1.0f, 1.0f,
				0.0f, 1.0f,
				0.0f, 1.0f,
				0.0f, 0.0f,
				1.0f, 0.0f,

				1.0f, 0.0f,
				1.0f, 1.0f,
				0.0f, 1.0f,
				0.0f, 1.0f,
				0.0f, 0.0f,
				1.0f, 0.0f,

				0.0f, 1.0f,
				1.0f, 1.0f,
				1.0f, 0.0f,
				1.0f, 0.0f,
				0.0f, 0.0f,
				0.0f, 1.0f,

				0.0f, 1.0f,
				1.0f, 1.0f,
				1.0f, 0.0f,
				1.0f, 0.0f,
				0.0f, 0.0f,
				0.0f, 1.0f
			};

			for (int i = 0; i < 36; i++) {
				vertices.push_back(Vertex(glm::vec3(coords[i * 3], coords[i * 3 + 1], coords[i * 3 + 2]),
					glm::vec3(normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]), glm::vec2(tex_coords[i * 2], tex_coords[i * 2 + 1])));
			}
			for (int i = 0; i < 36; ++i)
				indices.push_back(i);
		}
	};

	struct SkyBoxResource {

		SkyTextureResource skyTextureResource;
		std::vector<Vertex> vertices;
		std::vector<Texture> textures;
		std::vector<unsigned int> indices;

		void init() {
			skyTextureResource.init();

			float coords[] = {
				// positions          
				-1.0f,  1.0f, -1.0f,
				-1.0f, -1.0f, -1.0f,
				1.0f, -1.0f, -1.0f,
				1.0f, -1.0f, -1.0f,
				1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,

				-1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,

				1.0f, -1.0f, -1.0f,
				1.0f, -1.0f,  1.0f,
				1.0f,  1.0f,  1.0f,
				1.0f,  1.0f,  1.0f,
				1.0f,  1.0f, -1.0f,
				1.0f, -1.0f, -1.0f,

				-1.0f, -1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				1.0f,  1.0f,  1.0f,
				1.0f,  1.0f,  1.0f,
				1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,

				-1.0f,  1.0f, -1.0f,
				1.0f,  1.0f, -1.0f,
				1.0f,  1.0f,  1.0f,
				1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f, -1.0f,

				-1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				1.0f, -1.0f, -1.0f,
				1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				1.0f, -1.0f,  1.0f
			};

			textures.push_back(skyTextureResource.cubemapTexture);

			for (int i = 0; i < 36; i++) {
				vertices.push_back(Vertex(glm::vec3(coords[i * 3], coords[i * 3 + 1], coords[i * 3 + 2])));
			}
			for (int i = 0; i < 36; ++i)
				indices.push_back(i);
		 }
	};

	Resource() {
		init();
	}

	void init() {
		
	}
};




