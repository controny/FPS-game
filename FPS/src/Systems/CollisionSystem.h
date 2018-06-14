#pragma once
#include <GLFW/glfw3.h>

#include <ECS.h>
#include <Components/CollisionComponent.h>
#include <Components/PositionComponent.h>
#include <Components/MovementComponent.h>
#include <Components/PlayerComponent.h>
#include <Events/HitEvent.h>

//#define DEBUG_CHECK_COLLISION
//#define DEBUG_CHECK_RAY_COLLISION

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

struct Ray {
    glm::vec3 origin, direction;

    Ray() {
        origin = glm::vec3(0);
        direction = glm::vec3(0);
    }

    Ray(glm::vec3 origin, glm::vec3 direction) {
        this->origin = origin;
        this->direction = direction;
    }

    glm::vec3 getPoint(float t) {
        return origin + t * direction;
    }
};

// 碰撞检测
class CollisionSystem : public EntitySystem,
    public EventSubscriber<MousePressEvent> {
public:

    CollisionSystem() {}

    virtual void configure(class World* world) override
    {
        world->subscribe<MousePressEvent>(this);
    }

    virtual void unconfigure(class World* world) override
    {
        world->unsubscribeAll(this);
    }

    virtual void receive(class World* world, const MousePressEvent& event) override
    {
        world->each<PlayerComponent>([&](Entity* ent, ComponentHandle<PlayerComponent> playerCHandle) -> void {
            auto positionCHandle = ent->get<PositionComponent>();
            glm::vec3 origin = ent->get<CameraComponent>()->Position;
            glm::vec3 direction = positionCHandle->Front;
            //printf("origin: (%f, %f, %f)\n", origin.x, origin.y, origin.z);
            //printf("direction: (%f, %f, %f)\n", direction.x, direction.y, direction.z);
            Ray ray(origin, direction);
            bool shooted = false;
            world->each<CollisionComponent>(
                [&](Entity* other_ent,
                    ComponentHandle<CollisionComponent> collisionCHandle) -> void {
                if (ent->getEntityId() == other_ent->getEntityId() || shooted)
                    return;
                // check collision between ray and AABB body
                glm::vec2 t = checkRayBodyCollision(ray, getCurrentBody(other_ent));
                if (t[0] < t[1] && t[0] > 0.0f) {
                    glm::vec3 hitPos = ray.getPoint(t[0]);
                    shooted = true;

#ifdef DEBUG_CHECK_RAY_COLLISION
                    printf("----------------------\n");
                    printf("ray hit body(entity id: %d)\n", other_ent->getEntityId());
                    printf("hit position: (%f, %f, %f)\n", hitPos.x, hitPos.y, hitPos.z);
                    printf("----------------------\n");
#endif // DEBUG_CHECK_RAY_COLLISION

                    world->emit<HitEvent>({ other_ent, hitPos, direction });
                }
            });
        });
    }

    CollisionState checkBodyCollision(Entity* entity1, Entity* entity2) {
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

    glm::vec2 checkRayBodyCollision(Ray& ray, AABB body) {
        if (abs(ray.direction.x) < 0.000001f) //If the ray parallel to the YZ plane  
        {
            //If the ray is not within AABB box, then no collision
            if (ray.origin.x < body.x_min || ray.origin.x > body.x_max)
                return glm::vec2(1.0f, 0.0f);
        }
        if (abs(ray.direction.y) < 0.000001f) //If the ray parallel to the XZ plane  
        {
            //If the ray is not within AABB box, then no collision
            if (ray.origin.y < body.y_min || ray.origin.y > body.y_max)
                return glm::vec2(1.0f, 0.0f);
        }
        if (abs(ray.direction.z) < 0.000001f) //If the ray parallel to the XY plane  
        {
            //If the ray is not within AABB box, then no collision
            if (ray.origin.z < body.z_min || ray.origin.z > body.z_max)
                return glm::vec2(1.0f, 0.0f);
        }
        glm::vec3 inv_dir = 1.0f / ray.direction;
        glm::vec3 tMin = (body.getMinCoord() - ray.origin) * inv_dir;
        glm::vec3 tMax = (body.getMaxCoord() - ray.origin) * inv_dir;
        glm::vec3 t1 = glm::min(tMin, tMax);
        glm::vec3 t2 = glm::max(tMin, tMax);
        float tNear = max(max(t1.x, t1.y), t1.z);
        float tFar = min(min(t2.x, t2.y), t2.z);

        return glm::vec2(tNear, tFar);
    }

    AABB getPreviousBody(Entity* entity, AABB& curBody) {
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
                // check collision between two entities with CollisionComponent
                CollisionState state = checkBodyCollision(ent, other_ent);
                if (state.collisionInX || state.collisionInY || state.collisionInZ) {
                    //printf("collision\n");
                    stopMoving(ent, state);
                }              
            });
        });
    }
};
