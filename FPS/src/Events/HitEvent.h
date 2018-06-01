#pragma once

#include <ECS.h>

using namespace ECS;

struct HitEvent {
    Entity* hit_entity;
    glm::vec3 hit_position;
};