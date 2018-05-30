#pragma once
#include <GLFW/glfw3.h>

#include <ECS.h>
#include <Components/CollisionComponent.h>
#include <Components/PositionComponent.h>
#include <Components/MovementComponent.h>

//#define DEBUG_CHECK_COLLISION

using namespace ECS;

struct CollisionState {
    bool collisionInX;
    bool collisionInY;
    bool collisionInZ;

    CollisionState(bool _collisionInX, bool _collisionInY, bool _collisionInZ) {
        collisionInX = _collisionInX;
        collisionInY = _collisionInY;
        collisionInZ = _collisionInZ;
    }
};

// 碰撞检测
class CollisionSystem : public EntitySystem {
public:

    CollisionSystem() {}

    CollisionState checkCollision(Entity* entity1, Entity* entity2) {
        AABB curBody1 = getCurrentBody(entity1);
        AABB curBody2 = getCurrentBody(entity2);
        // collision detection on the XYZ axes after moving
        bool curIntersectX = (curBody1.x_max >= curBody2.x_min) && (curBody2.x_max >= curBody1.x_min);
        bool curIntersectY = (curBody1.y_max >= curBody2.y_min) && (curBody2.y_max >= curBody1.y_min);
        bool curIntersectZ = (curBody1.z_max >= curBody2.z_min) && (curBody2.z_max >= curBody1.z_min);
        
        if (!(curIntersectX && curIntersectY && curIntersectZ)) {
            return CollisionState{ false, false, false };
        }

        AABB preBody1 = getPreviousBody(entity1, curBody1);
        AABB preBody2 = getPreviousBody(entity2, curBody2);
        // collision detection on the XYZ axes before moving
        bool preIntersectX = (preBody1.x_max >= preBody2.x_min) && (preBody2.x_max >= preBody1.x_min);
        bool preIntersectY = (preBody1.y_max >= preBody2.y_min) && (preBody2.y_max >= preBody1.y_min);
        bool preIntersectZ = (preBody1.z_max >= preBody2.z_min) && (preBody2.z_max >= preBody1.z_min);
        bool collisionInX = (curIntersectX==true) && (preIntersectX==false);
        bool collisionInY = (curIntersectY==true) && (preIntersectY==false);
        bool collisionInZ = (curIntersectZ==true) && (preIntersectZ==false);

#ifdef DEBUG_CHECK_COLLISION
        printf("-----------------\n");
        printf("curIntersectX: %d\n", curIntersectX);
        printf("curIntersectY: %d\n", curIntersectY);
        printf("curIntersectZ: %d\n", curIntersectZ);
        printf("preIntersectX: %d\n", preIntersectX);
        printf("preIntersectY: %d\n", preIntersectY);
        printf("preIntersectZ: %d\n", preIntersectZ);
        printf("collisionInX: %d\n", collisionInX);
        printf("collisionInY: %d\n", collisionInY);
        printf("collisionInZ: %d\n", collisionInZ);
        printf("-----------------\n");
#endif

        return CollisionState{ collisionInX, collisionInY, collisionInZ };
    }

    AABB getPreviousBody(Entity* entity, AABB curBody) {
        if (!entity->has<MovementComponent>()) {
            return curBody;
        }
        return curBody - entity->get<MovementComponent>()->shift;
    }

    AABB getCurrentBody(Entity* entity) {
        auto body = entity->get<CollisionComponent>()->body;
        auto pos  = entity->get<PositionComponent>()->Position;
        float x_min = body.x_min + pos.x;
        float x_max = body.x_max + pos.x;
        float y_min = body.y_min + pos.y;
        float y_max = body.y_max + pos.y;
        float z_min = body.z_min + pos.z;
        float z_max = body.z_max + pos.z;
        AABB currentBody = { x_min, x_max, y_min, y_max, z_min, z_max };
        return currentBody;
    }

    void stopMoving(Entity* entity, CollisionState& state) {
        //printf("stop moving\n");
        auto movementCHandle = entity->get<MovementComponent>();
        auto positionCHandle = entity->get<PositionComponent>();
        if (state.collisionInX) {
            movementCHandle->Velocity.x = 0.0f;
        }
        if (state.collisionInY) {
            movementCHandle->Velocity.y = 0.0f;
            if (entity->has<PlayerComponent>())
                entity->get<PlayerComponent>()->isJumping = false;
        }
        if (state.collisionInZ) {
            movementCHandle->Velocity.z = 0.0f;
        }
        // repositioning
        positionCHandle->Position -= movementCHandle->shift;
    }

    // 每次轮询到时，对所有添加了CollisionComponent的entity进行碰撞检测
    virtual void tick(class World* world, float deltaTime) override {
        int num_entities = world->getCount();
        
        world->each<MovementComponent, CollisionComponent>(
        [&](Entity* ent,
            ComponentHandle<MovementComponent> movementCHandle,
            ComponentHandle<CollisionComponent> collisionCHandle) -> void {
            
            world->each<CollisionComponent>(
            [&](Entity* other_ent,
                ComponentHandle<CollisionComponent> collisionCHandle) -> void {             
                if (ent->getEntityId() == other_ent->getEntityId())
                    return;
                // check collision between two entities
                CollisionState state = checkCollision(ent, other_ent);
                if (state.collisionInX || state.collisionInY || state.collisionInZ) {
                    //printf("collision\n");
                    stopMoving(ent, state);
                }              
            });
        });
    }
};
