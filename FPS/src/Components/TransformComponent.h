#pragma once
#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct TransformComponent {
	glm::vec3 translate;
	glm::vec3 scale;
	glm::vec3 Front;
	glm::vec3 Right;
	glm::vec3 Up;
	TransformComponent(glm::vec3 trans = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 sca=glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 front=glm::vec3(0.0f, 0.0f, 1.0f)) {
		translate = trans;
		scale = sca;
		Front = front;
	}
};