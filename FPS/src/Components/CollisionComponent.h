#pragma once

#include <GLFW/glfw3.h>
#include <ECS.h>
#include <glm/glm.hpp>

#include <string>

using namespace ECS;
using namespace std;

struct AABB {
    float x_min;
    float x_max;
    float y_min;
    float y_max;
    float z_min;
    float z_max;

    AABB() {}

    AABB(const float &_Xmin, const float &_Xmax,
         const float &_Ymin, const float &_Ymax,
         const float &_Zmin, const float &_Zmax) {
        x_min = _Xmin;
        x_max = _Xmax;
        y_min = _Ymin;
        y_max = _Ymax;
        z_min = _Zmin;
        z_max = _Zmax;
    }

    AABB operator-(glm::vec3 shift) {
        AABB tmp;
        tmp.x_min = x_min - shift.x;
        tmp.x_max = x_max - shift.x;
        tmp.y_min = y_min - shift.y;
        tmp.y_max = y_max - shift.y;
        tmp.z_min = z_min - shift.z;
        tmp.z_max = z_max - shift.z;
        return tmp;
    }
};

// CollisionComponent: store the physics body of a entity
struct CollisionComponent {
    
    AABB body;
    float original_width;
    float original_depth;
    float original_height;

    CollisionComponent(float width, float depth, float height) {
        original_width = width;
        original_depth = depth;
        original_height = height;
        float halfExtents_X = width / 2;
        float halfExtents_Y = height / 2;
        float halfExtents_Z = depth / 2;
        body.x_min = -halfExtents_X;
        body.x_max =  halfExtents_X;
        body.y_min = -halfExtents_Y;
        body.y_max =  halfExtents_Y;
        body.z_min = -halfExtents_Z;
        body.z_max =  halfExtents_Z;
    }
    
    CollisionComponent(float x_min, float x_max,
                       float y_min, float y_max,
                       float z_min, float z_max) {
        original_width = abs(x_max - x_min);
        original_depth = abs(z_max - z_min);
        original_height = abs(y_max - y_min);
        body.x_min = x_min;
        body.x_max = x_max;
        body.y_min = y_min;
        body.y_max = y_max;
        body.z_min = z_min;
        body.z_max = z_max;
    }

    void updateBody(float width, float depth) {
        float halfExtents_X = width / 2;
        float halfExtents_Z = depth / 2;
        body.x_min = -halfExtents_X;
        body.x_max =  halfExtents_X;
        body.z_min = -halfExtents_Z;
        body.z_max =  halfExtents_Z;
    }

    void Print() {
        printf("body(x_min, x_max, y_min, y_max, z_min, z_max): (%f, %f, %f, %f, %f, %f)\n", 
                body.x_min, body.x_max, body.y_min, body.y_max, body.z_min, body.z_max);
    }
};
