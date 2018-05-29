#pragma once

#include <GLFW/glfw3.h>
#include <ECS.h>

#include <glm/glm.hpp>

#include <cstdio>


// MovementComponent: store the movement infomation (velocity & acceleration)
struct MovementComponent {

    glm::vec3 Velocity;
    glm::vec3 Acceleration;
    glm::vec3 shift;

    MovementComponent(glm::vec3 _Velocity, glm::vec3 _Acceleration) {
        Velocity     = _Velocity;
        Acceleration = _Acceleration;
        shift        = glm::vec3(0.0f, 0.0f, 0.0f);
    }

    void Print() {
        printf("Velocity: (%f, %f, %f)\n", Velocity.x, Velocity.y, Velocity.z);
        printf("Acceleration: (%f, %f, %f)\n", Acceleration.x, Acceleration.y, Acceleration.z);
    }
};
