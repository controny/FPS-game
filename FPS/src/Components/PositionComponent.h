#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <cstdio>

// PositioinComponent: store the position of each entity
struct PositionComponent {

    glm::vec3 Position;

    PositionComponent(glm::vec3 _Position) {
        Position = _Position;
    }

    void Print() {
        printf("(%f, %f, %f)", Position.x, Position.y, Position.z);
    }
};
