#pragma once
#include <GLFW/glfw3.h>

#include <ECS.h>
#include <Components/PositionComponent.h>
#include <Components/MovementComponent.h>

using namespace ECS;

// 根据Movement数据，更新entity的Position数据
class MovementSystem : public EntitySystem {
public:

    MovementSystem() {}

    // 每次轮询到时，为每一个添加了MovementComponent的entity计算位移数据，并更新Position数据
    virtual void tick(class World* world, float deltaTime) override {
        world->each<MovementComponent, PositionComponent>(
        [&](Entity* ent,
            ComponentHandle<MovementComponent> movementCHandle,
            ComponentHandle<PositionComponent> positionCHandle) -> void {
            glm::vec3 v = movementCHandle->Velocity;
            glm::vec3 a = movementCHandle->Acceleration;
            glm::vec3 shift = v * deltaTime;
            movementCHandle->Velocity = v + a * deltaTime;
            positionCHandle->Position += shift;
        });
    }
};