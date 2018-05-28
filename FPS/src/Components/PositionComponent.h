#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <cstdio>

// PositioinComponent: store the position of a entity's center
struct PositionComponent {

    glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Right;
	glm::vec3 Up;

    PositionComponent(glm::vec3 _Position) {
        Position = _Position;
    }

    void Print() {
        printf("(%f, %f, %f)\n", Position.x, Position.y, Position.z);
    }
};
