#pragma once
#include <GLFW/glfw3.h>

#include <ECS.h>
#include <Components/PositionComponent.h>
#include <Components/MovementComponent.h>
#include <Components/ObjectComponent.h>

using namespace ECS;

// 根据Movement数据，更新entity的Position数据
class MovementSystem : public EntitySystem {
public:

    MovementSystem() {}

    void updateVertices(vector<Vertex> &vertices, glm::vec3 shift) {
        for (int i = 0; i < vertices.size(); ++i) {
            vertices[i].Position += shift;
        }
    }

    // 每次轮询到时，为每一个添加了MovementComponent的entity计算位移数据，并更新Position数据
    virtual void tick(class World* world, float deltaTime) override {
        world->each<MovementComponent, ObjectComponent>(
        [&](Entity* ent,  
            ComponentHandle<MovementComponent> movementCHandle,
            ComponentHandle<ObjectComponent> objectCHandle) -> void {
            glm::vec3 v = movementCHandle->Velocity;
            glm::vec3 a = movementCHandle->Acceleration;
            glm::vec3 shift = v * deltaTime;
            movementCHandle->Velocity = v + a * deltaTime;
            //printf("(%f, %f, %f)\n", shift.x, shift.y, shift.z);
            //movementCHandle->Print();
            // 更新vertices中每一个Vertex的Position数据并传送到GPU缓存
            for (int i = 0; i < objectCHandle->meshes[0].vertices.size(); ++i) {
                objectCHandle->meshes[0].vertices[i].Position += shift;
            }
            objectCHandle->meshes[0].updateVerticesInGPU();
        });
    }
};