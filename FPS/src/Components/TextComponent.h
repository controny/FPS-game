#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H

#include <GLFW/glfw3.h>

#include <ECS.h>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <map>
#include <vector>
#include <iostream>


struct Character {
	GLuint TextureID;   // ID handle of the glyph texture
	glm::ivec2 Size;    // Size of glyph
	glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
	GLuint Advance;    // Horizontal offset to advance to next glyph
};

struct TextComponent {

	string info;
	static std::map<char, Character> Characters;
	vector<GLuint> VAOs, VBOs, TextureIDs;
	string text;
	glm::vec3 color;
	GLfloat x, y, scale;  // 比例
	GLfloat x_pos, y_pos, size_scale;  // 实际像素位置，根据 window 大小算出
	string font_dir;

	FT_Library ft;
	FT_Face face;

	TextComponent(string info, string text, GLfloat _x, GLfloat _y, GLfloat _scale, GLfloat _window_width, GLfloat _window_height, glm::vec3 _color, string font_dir) {
		init(info, text, _x, _y, _scale, _window_width, _window_height, _color, font_dir);
	}

	void setText(string _text) {
		this->text = _text;
		refreshBuffers();
	}

	void setPos(float window_width, float window_height) {
		this->x_pos = window_width * this->x;
		this->y_pos = window_height * this->y;
		this->size_scale = min(window_width, window_height) / 600.0f * scale;
		refreshBuffers();
		//cout << x_pos << ' ' << y_pos << endl;
	}

	void refreshBuffers() {
		VAOs.resize(text.size());
		VBOs.resize(text.size());
		TextureIDs.resize(text.size());

		for (int i = 0; i < text.size(); i++) {
			glGenVertexArrays(1, &VAOs[i]);
			glGenBuffers(1, &VBOs[i]);
			glBindVertexArray(VAOs[i]);
			glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}

		// init VAOs, VBOs
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		float ori_x = x_pos, ori_y = y_pos;
		for (int i = 0; i < text.size(); i++)
		{
			glBindVertexArray(this->VAOs[i]);
			Character ch = this->Characters[text[i]];
			TextureIDs[i] = this->Characters[text[i]].TextureID;

			GLfloat xpos = ori_x + ch.Bearing.x * size_scale;
			GLfloat ypos = ori_y - (ch.Size.y - ch.Bearing.y) * size_scale;

			GLfloat w = ch.Size.x * size_scale;
			GLfloat h = ch.Size.y * size_scale;
			// Update VBO for each character
			GLfloat vertices[6][4] = {
				{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
			};
			// Render glyph texture over quad
			glBindTexture(GL_TEXTURE_2D, ch.TextureID);
			// Update content of VBO memory
			glBindBuffer(GL_ARRAY_BUFFER, this->VBOs[i]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, vertices, GL_STATIC_DRAW);// Be sure to use glBufferSubData and not glBufferData
																							 //glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

																							 // Render quad
			glDrawArrays(GL_TRIANGLES, 0, 6);
			// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
			ori_x += (ch.Advance >> 6) * size_scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
		}

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void init(string _info, string _text, GLfloat _x, GLfloat _y, GLfloat _scale, GLfloat _window_width, GLfloat _window_height, glm::vec3 _color, string _font_dir) {
		info = _info;
		color = _color;
		x = _x;
		y = _y;
		x_pos = y_pos = 0;
		scale = _scale;
		text = _text;
		font_dir = _font_dir;
		
		if (Characters.size() == 0) {
			// All functions return a value different than 0 whenever an error occurred
			if (FT_Init_FreeType(&ft))
				std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

			// Load font as face
			if (FT_New_Face(ft, (font_dir + "inso.ttf").c_str(), 0, &face))
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
		
		setPos(_window_width, _window_height);
		refreshBuffers();
	}
};

std::map<char, Character> TextComponent::Characters;
