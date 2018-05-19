#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H

#include <GLFW/glfw3.h>

#include <ECS.h>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <map>
#include <iostream>


struct Character {
	GLuint TextureID;   // ID handle of the glyph texture
	glm::ivec2 Size;    // Size of glyph
	glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
	GLuint Advance;    // Horizontal offset to advance to next glyph
};

struct TextComponent {

	static std::map<char, Character> Characters;
	GLuint VAO, VBO;

	string text;
	float x, y;
	float scale;
	glm::vec3 color;

	TextComponent(std::string _text, GLfloat _x, GLfloat _y, GLfloat _scale, glm::vec3 _color) {
		init(_text, _x, _y, _scale, _color);
	}

	void init(std::string _text, GLfloat _x, GLfloat _y, GLfloat _scale, glm::vec3 _color) {
		text = _text;
		x = _x;
		y = _y;
		scale = _scale;
		color = _color;

		// init ft
		if (Characters.size() == 0) {
			FT_Library ft;
			// All functions return a value different than 0 whenever an error occurred
			if (FT_Init_FreeType(&ft))
				std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

			// Load font as face
			FT_Face face;
			if (FT_New_Face(ft, "resources/fonts/arial.ttf", 0, &face))
				std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

			// Set size to load glyphs as
			FT_Set_Pixel_Sizes(face, 0, 48);

			// Disable byte-alignment restriction
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			for (GLubyte c = 0; c < 128; c++)
			{
				// Load character glyph 
				if (FT_Load_Char(face, c, FT_LOAD_RENDER))
				{
					std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
					continue;
				}
				// Generate texture
				GLuint texture;
				glGenTextures(1, &texture);
				glBindTexture(GL_TEXTURE_2D, texture);
				glTexImage2D(
					GL_TEXTURE_2D,
					0,
					GL_RED,
					face->glyph->bitmap.width,
					face->glyph->bitmap.rows,
					0,
					GL_RED,
					GL_UNSIGNED_BYTE,
					face->glyph->bitmap.buffer
				);
				// Set texture options
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				// Now store character for later use
				Character character = {
					texture,
					glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
					glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
					face->glyph->advance.x
				};
				Characters.insert(std::pair<char, Character>(c, character));
			}
			glBindTexture(GL_TEXTURE_2D, 0);
			// Destroy FreeType once we're finished
			FT_Done_Face(face);
			FT_Done_FreeType(ft);
		}
		
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
};

std::map<char, Character> TextComponent::Characters;
