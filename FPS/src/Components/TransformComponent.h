#pragma once
#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct TransformComponent {

	glm::vec3 translate;
	glm::vec3 relative_translate;
	glm::vec3 scale;

	float rotate_x;
	float rotate_y;

	TransformComponent(glm::vec3 trans = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 sca=glm::vec3(0.0f, 0.0f, 0.0f), float _rotate_x = 0.0f, float _rotate_y = 0.0f) {
		translate = trans;
		relative_translate = glm::vec3(0.0f, 0.0f, 0.0f);
		scale = sca;
		rotate_x = _rotate_x;
		rotate_y = _rotate_y;
	}
};