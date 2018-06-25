#pragma once

#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <ECS.h>
#include <Components/WindowInfoSingletonComponent.h>


using namespace ECS;


// 之后 assign 给每个枪的 entity，从而不同的枪能拥有不同的准心类型
struct PostComponent {
	float length;  // 准心那条线的长度
	float size;  // 准心大小，越小越准
	glm::vec3 Color;
	GLuint line_VAO, line_VBO, point_VAO, point_VBO;
	float window_rate;
	

	float vertices[24] = {
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f, 

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,

		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
	};

	PostComponent(glm::vec3 _color, float _size, float _length = 0.02f) {
		init(_color, _size, _length);
	}

	void init(glm::vec3 _color, float _size, float _window_rate = 1.33, float _length = 0.02f) {
		Color = _color;
		glGenVertexArrays(1, &line_VAO);
		glGenBuffers(1, &line_VBO);
		glBindVertexArray(line_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, line_VBO);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		float point[3] = { 0.0f, 0.0f, 0.0f };
		glGenVertexArrays(1, &point_VAO);
		glGenBuffers(1, &point_VBO);
		glBindVertexArray(point_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, point_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(point), &point[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		length = _length;
		window_rate = _window_rate;
		setSize(_size, window_rate);
	}

	// 转换成设备坐标后，坐标位置根据窗口大小来定，不能变成固定大小的正方形。要我改变窗口大小，准心大小也不变。
	void setSize(float _size, float _window_rate) {
		window_rate = _window_rate;
		/*设置size2,length2大小来通过窗口比例实现准心正方形*/
		float length2 = length * _window_rate;
		size = _size;
		float size2 = size * _window_rate;

		vertices[0] = -size;
		vertices[3] = -size - length;

		vertices[6] = size;
		vertices[9] = size + length;

		vertices[13] = size2;
		vertices[16] = size2 + length2;

		vertices[19] = -size2;
		vertices[22] = -size2 - length2;

		glBindVertexArray(line_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, line_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
};