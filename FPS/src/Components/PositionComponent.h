#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <cstdio>

// PositioinComponent: store the position of a entity's center
struct PositionComponent {

    glm::vec3 Position;

	// 如果不需要旋转，下面三个可以不管；暂时只有 player 视角移动的时候才有用
	glm::vec3 Front;
	glm::vec3 Right;
	glm::vec3 Up;

    PositionComponent(glm::vec3 _Position) {
        Position = _Position;
    }

    void Print() {
        printf("Position: (%f, %f, %f)\n", Position.x, Position.y, Position.z);
    }
};
