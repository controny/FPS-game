#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <GLFW/glfw3.h>
#include <loadpng/lodepng.h>

#include <iostream>
#include <string>

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

GLuint loadPNG(const char *name, bool highQualityMipmaps)
{
	unsigned int width;
	unsigned int height;
	unsigned char *data;
	unsigned char *temp;
	unsigned int i;
	GLuint result = 0;

	// use a simple stand-alone library to load our PNGs
	lodepng_decode32_file(&data, &width, &height, name);

	// make sure the load was successful
	if (data)
	{
		// the pixel data is flipped vertically, so we need to flip it back with an in-place reversal
		temp = new unsigned char[width * 4];
		for (i = 0; i < height / 2; i++)
		{
			memcpy(temp, &data[i * width * 4], (width * 4));								// copy row into temp array
			memcpy(&data[i * width * 4], &data[(height - i - 1) * width * 4], (width * 4));	// copy other side of array into this row
			memcpy(&data[(height - i - 1) * width * 4], temp, (width * 4));					// copy temp into other side of array
		}

		// we can generate a texture object since we had a successful load
		glGenTextures(1, &result);
		glBindTexture(GL_TEXTURE_2D, result);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		// texture UVs should not clamp
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// generate high-quality mipmaps for this texture?
		if (highQualityMipmaps)
		{
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		}
		else
		{
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		}

		// release the memory used to perform the loading
		delete[] data;
		delete[] temp;
	}
	else
	{
		cerr << "loadTexture() could not load " << name << endl;
		exit(1);
	}

	return result;
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

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII
#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT 0x83F0
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3
GLuint loadDDS(const char * imagepath) {

	unsigned char header[124];

	FILE *fp;

	/* try to open the file */
	fp = fopen(imagepath, "rb");
	if (fp == NULL) {
		printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath); getchar();
		return 0;
	}

	/* verify the type of file */
	char filecode[4];
	fread(filecode, 1, 4, fp);
	if (strncmp(filecode, "DDS ", 4) != 0) {
		fclose(fp);
		return 0;
	}

	/* get the surface desc */
	fread(&header, 124, 1, fp);

	unsigned int height = *(unsigned int*)&(header[8]);
	unsigned int width = *(unsigned int*)&(header[12]);
	unsigned int linearSize = *(unsigned int*)&(header[16]);
	unsigned int mipMapCount = *(unsigned int*)&(header[24]);
	unsigned int fourCC = *(unsigned int*)&(header[80]);


	unsigned char * buffer;
	unsigned int bufsize;
	/* how big is it going to be including all mipmaps? */
	bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
	buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char));
	fread(buffer, 1, bufsize, fp);
	/* close the file pointer */
	fclose(fp);

	unsigned int components = (fourCC == FOURCC_DXT1) ? 3 : 4;
	unsigned int format;
	switch (fourCC)
	{
	case FOURCC_DXT1:
		format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		break;
	case FOURCC_DXT3:
		format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		break;
	case FOURCC_DXT5:
		format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		break;
	default:
		free(buffer);
		return 0;
	}

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
	unsigned int offset = 0;

	/* load the mipmaps */
	for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
	{
		unsigned int size = ((width + 3) / 4)*((height + 3) / 4)*blockSize;
		glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,
			0, size, buffer + offset);

		offset += size;
		width /= 2;
		height /= 2;

		// Deal with Non-Power-Of-Two textures. This code is not included in the webpage to reduce clutter.
		if (width < 1) width = 1;
		if (height < 1) height = 1;

	}

	free(buffer);

	return textureID;
}

struct Resource {

	static string resource_dir;

	struct TextureResource {
		Texture container_diffuse;
		Texture container_specular;

		// textures for ground
		Texture ground_diffuse;
		Texture ground_specular;
        Texture ground_albedo;
        Texture ground_normal;
        Texture ground_metallic;
        Texture ground_roughness;
        Texture ground_ao;
        Texture ground_height;

        Texture wall_albedo;
        Texture wall_normal;
        Texture wall_metallic;
        Texture wall_roughness;
        Texture wall_ao;
        Texture wall_height;

		void init() {
			container_diffuse = Texture(Load((resource_dir + "textures/container2.png").c_str()), "texture_diffuse");
			container_specular = Texture(Load((resource_dir + "textures/container2_specular.png").c_str()), "texture_specular");

			ground_diffuse = Texture(Load((resource_dir + "textures/wood.png").c_str()), "texture_diffuse");
			ground_specular = Texture(Load((resource_dir + "textures/wood_specular.jpg").c_str()), "texture_specular");

            ground_albedo = Texture(Load((resource_dir + "textures/marble-speckled/marble-speckled-albedo.png").c_str()), "albedoMap");
            ground_normal = Texture(Load((resource_dir + "textures/marble-speckled/marble-speckled-normal.png").c_str()), "normalMap");
            ground_metallic = Texture(Load((resource_dir + "textures/marble-speckled/marble-speckled-metalness.png").c_str()), "metallicMap");
            ground_roughness = Texture(Load((resource_dir + "textures/marble-speckled/marble-speckled-roughness.png").c_str()), "roughnessMap");

            wall_albedo = Texture(Load((resource_dir + "textures/blocksrough/blocksrough_basecolor.png").c_str()), "albedoMap");
            wall_normal = Texture(Load((resource_dir + "textures/blocksrough/blocksrough_normal.png").c_str()), "normalMap");
            wall_metallic = Texture(Load((resource_dir + "textures/blocksrough/blocksrough_metallic.png").c_str()), "metallicMap");
            wall_roughness = Texture(Load((resource_dir + "textures/blocksrough/blocksrough_roughness.png").c_str()), "roughnessMap");
            wall_ao = Texture(Load((resource_dir + "textures/blocksrough/blocksrough_ambientocclusion.png").c_str()), "aoMap");
            wall_height = Texture(Load((resource_dir + "textures/blocksrough/blocksrough_height.png").c_str()), "heightMap");
		}
	};

	struct SkyTextureResource {
		Texture cubemapTexture;
		vector<std::string> faces
		{
			/*
			resource_dir + "textures/skybox/bluesky/right.jpg",
			resource_dir + "textures/skybox/bluesky/left.jpg",
			resource_dir + "textures/skybox/bluesky/top.jpg",
			resource_dir + "textures/skybox/bluesky/bottom.jpg",
			resource_dir + "textures/skybox/bluesky/back.jpg",
			resource_dir + "textures/skybox/bluesky/front.jpg"
			*/
			/*
			resource_dir + "textures/skybox/iceflow/iceflow_rt.tga",
			resource_dir + "textures/skybox/iceflow/iceflow_lf.tga",
			resource_dir + "textures/skybox/iceflow/iceflow_up.jpg",
			resource_dir + "textures/skybox/iceflow/iceflow_dn.tga",
			resource_dir + "textures/skybox/iceflow/iceflow_bk.tga",
			resource_dir + "textures/skybox/iceflow/iceflow_ft.tga"
			*/
			/*
			resource_dir + "textures/skybox/arrakisday/arrakisday_rt.tga",
			resource_dir + "textures/skybox/arrakisday/arrakisday_lf.tga",
			resource_dir + "textures/skybox/arrakisday/arrakisday_up.jpg",
			resource_dir + "textures/skybox/arrakisday/arrakisday_dn.tga",
			resource_dir + "textures/skybox/arrakisday/arrakisday_bk.tga",
			resource_dir + "textures/skybox/arrakisday/arrakisday_ft.tga"
			*/
			resource_dir + "textures/skybox/ame_cotton/cottoncandy_rt.tga",
			resource_dir + "textures/skybox/ame_cotton/cottoncandy_lf.tga",
			resource_dir + "textures/skybox/ame_cotton/cottoncandy_up.jpg",
			resource_dir + "textures/skybox/ame_cotton/cottoncandy_dn.tga",
			resource_dir + "textures/skybox/ame_cotton/cottoncandy_bk.tga",
			resource_dir + "textures/skybox/ame_cotton/cottoncandy_ft.tga"
			
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

		void init(float x, float y, float z,
			Texture diffuse_texture, Texture specular_texture) {

			vertices.clear();
			indices.clear();
			textures.clear();

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
				if ((i + 1) % 3 == 1) coords[i] = coords[i] * x;
				else if ((i + 1) % 3 == 2) coords[i] = coords[i] * y;
				else coords[i] = coords[i] * z;
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

    struct PBR_CubeResource {

        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        void init(float x, float y, float z, float tex_coords_value,
            Texture albedo_texture,
            Texture normal_texture,
            Texture metallic_texture,
            Texture roughness_texture,
            Texture ao_texture,
            Texture height_texture) {

            vertices.clear();
            indices.clear();
            textures.clear();

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
                if ((i + 1) % 3 == 1) coords[i] = coords[i] * x;
                else if ((i + 1) % 3 == 2) coords[i] = coords[i] * y;
                else coords[i] = coords[i] * z;
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

            textures.push_back(albedo_texture);
            textures.push_back(normal_texture);
            textures.push_back(metallic_texture);
            textures.push_back(roughness_texture);
            textures.push_back(ao_texture);

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
                    glm::vec3(normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]),
                    glm::vec2(tex_coords[i * 2] * tex_coords_value, tex_coords[i * 2 + 1] * tex_coords_value)));
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

	struct ModelResource {
		vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
		vector<Mesh> meshes;
		string directory;

		ModelResource() {}

		ModelResource(string const &path)
		{
			loadModel(path);
		}

		void loadModel(string const &path)
		{
			// read file via ASSIMP
			Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
			// check for errors
			if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
			{
				cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
				return;
			}
			// retrieve the directory path of the filepath
			directory = path.substr(0, path.find_last_of('/'));

			// process ASSIMP's root node recursively
			processNode(scene->mRootNode, scene);
		}

		void processNode(aiNode *node, const aiScene *scene)
		{
			// process each mesh located at the current node
			for (unsigned int i = 0; i < node->mNumMeshes; i++)
			{
				// the node object only contains indices to index the actual objects in the scene. 
				// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
				meshes.push_back(processMesh(mesh, scene));
			}
			// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
			for (unsigned int i = 0; i < node->mNumChildren; i++)
			{
				processNode(node->mChildren[i], scene);
			}

		}

		Mesh processMesh(aiMesh *mesh, const aiScene *scene)
		{
			// data to fill
			vector<Vertex> vertices;
			vector<unsigned int> indices;
			vector<Texture> textures;

			// Walk through each of the mesh's vertices
			for (unsigned int i = 0; i < mesh->mNumVertices; i++)
			{
				Vertex vertex;
				glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
								  // positions
				vector.x = mesh->mVertices[i].x;
				vector.y = mesh->mVertices[i].y;
				vector.z = mesh->mVertices[i].z;
				vertex.Position = vector;
				// normals
				vector.x = mesh->mNormals[i].x;
				vector.y = mesh->mNormals[i].y;
				vector.z = mesh->mNormals[i].z;
				vertex.Normal = vector;
				// texture coordinates
				if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
				{
					glm::vec2 vec;
					// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
					// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
					vec.x = mesh->mTextureCoords[0][i].x;
					vec.y = mesh->mTextureCoords[0][i].y;
					vertex.TexCoords = vec;
				}
				else
					vertex.TexCoords = glm::vec2(0.0f, 0.0f);
				// tangent
				vector.x = mesh->mTangents[i].x;
				vector.y = mesh->mTangents[i].y;
				vector.z = mesh->mTangents[i].z;
				vertex.Tangent = vector;
				// bitangent
				vector.x = mesh->mBitangents[i].x;
				vector.y = mesh->mBitangents[i].y;
				vector.z = mesh->mBitangents[i].z;
				vertex.Bitangent = vector;
				vertices.push_back(vertex);
			}
			// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
			for (unsigned int i = 0; i < mesh->mNumFaces; i++)
			{
				aiFace face = mesh->mFaces[i];
				// retrieve all indices of the face and store them in the indices vector
				for (unsigned int j = 0; j < face.mNumIndices; j++)
					indices.push_back(face.mIndices[j]);
			}
			// process materials
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
			// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
			// Same applies to other texture as the following list summarizes:
			// diffuse: texture_diffuseN
			// specular: texture_specularN
			// normal: texture_normalN

			// 1. diffuse maps
			vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			// 2. specular maps
			vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
			// 3. normal maps
			std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
			// 4. height maps
			std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
			textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

			// return a mesh object created from the extracted mesh data
			return Mesh(vertices, indices, textures);
		}

		// checks all material textures of a given type and loads the textures if they're not loaded yet.
		// the required info is returned as a Texture struct.
		vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
		{
			vector<Texture> textures;
			for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
			{
				aiString str;
				mat->GetTexture(type, i, &str);
				// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
				bool skip = false;
				for (unsigned int j = 0; j < textures_loaded.size(); j++)
				{
					if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
					{
						textures.push_back(textures_loaded[j]);
						skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
						break;
					}
				}
				//if (!skip)
				{   // if texture hasn't been loaded already, load it
					Texture texture;
					texture.id = TextureFromFile(str.C_Str(), this->directory);
					texture.type = typeName;
					texture.path = str.C_Str();
					textures.push_back(texture);
					textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
				}
			}
			return textures;
		}

		unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false)
		{
			string filename = string(path);
			filename = directory + '/' + filename;

			unsigned int textureID;
			glGenTextures(1, &textureID);

			int width, height, nrComponents;
			unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
			if (data)
			{
				GLenum format;
				if (nrComponents == 1)
					format = GL_RED;
				else if (nrComponents == 3)
					format = GL_RGB;
				else if (nrComponents == 4)
					format = GL_RGBA;

				glBindTexture(GL_TEXTURE_2D, textureID);
				glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				stbi_image_free(data);
			}
			else
			{
				std::cout << "Texture failed to load at path: " << path << std::endl;
				stbi_image_free(data);
			}

			return textureID;
		}
	};

	struct ParticleResource {
		unsigned int bloodTexture, smokeTexture, gunFireTexture, disappearingTexture;

		void init(string path) {
			bloodTexture = loadDDS((path + "particle.DDS").c_str());
			smokeTexture = loadPNG((path + "smoke.png").c_str(), true);
			gunFireTexture = loadPNG((path + "muzzle-flash.png").c_str(), true);
			disappearingTexture = loadDDS((path + "particle.DDS").c_str());
		}
	};
	Resource(string _resource_dir) {
		init(_resource_dir);
	}

	void init(string _resource_dir) {
		resource_dir = _resource_dir;
	}
};

string Resource::resource_dir = "";




