#pragma once
#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct TransformComponent {
	glm::vec3 translate;
	glm::vec3 rotate;
	glm::vec3 scale;
	TransformComponent(glm::vec3 trans = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 rot = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 sca=glm::vec3(0.0f, 0.0f, 0.0f)) {
		translate = trans;
		rotate = rot;
		scale = sca;
	}
};