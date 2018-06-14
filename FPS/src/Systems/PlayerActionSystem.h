#pragma once
#include <GLFW/glfw3.h>

#include <ECS.h>
#include <Components/MovementComponent.h>
#include <Components/PlayerComponent.h>
#include <Components/ObjectComponent.h>
#include <Components/WindowInfoSingletonComponent.h>
#include <Components/LightingInfoSingletonComponent.h>
#include <Events/KeyEvents.h>

using namespace ECS;

// 根据键盘输入完成行走、跳等操作
class PlayerActionSystem : public EntitySystem,
	public EventSubscriber<KeyPressEvent>,
	public EventSubscriber<KeyReleaseEvent> {
public:

	float speed_multiples;
	bool forward, backward, left, right, speed_up, jump;

	virtual void configure(class World* world) override
	{
		world->subscribe<KeyPressEvent>(this);
		world->subscribe<KeyReleaseEvent>(this);
	}

	virtual void unconfigure(class World* world) override
	{
		world->unsubscribeAll(this);
	}

	virtual void receive(class World* world, const KeyPressEvent& event) override
	{	
		if (event.key == LEFT_SHIFT)
			this->speed_up = true;

		if (event.key == W)
			this->forward = true;
		if (event.key == S)
			this->backward = true;
		if (event.key == A)
			this->left = true;
		if (event.key == D)
			this->right = true;

        if (event.key == SPACE) {
            world->each<PlayerComponent>([&](Entity* ent, ComponentHandle<PlayerComponent> playerCHandle) -> void {
                this->jump = !playerCHandle->isJumping;
            });
        }
	}

	virtual void receive(class World* world, const KeyReleaseEvent& event) override
	{
		if (event.key == LEFT_SHIFT)
			this->speed_up = false;

		if (event.key == W)
			this->forward = false;
		if (event.key == S)
			this->backward = false;
		if (event.key == A)
			this->left = false;
		if (event.key == D)
			this->right = false;
	}

	void init() {

	}

	// 根据当前的按键情况计算出当前	速度
	virtual void tick(class World* world, float deltaTime) override {
		world->each<PlayerComponent>([&](Entity* ent, ComponentHandle<PlayerComponent> playerCHandle) -> void {
			ComponentHandle<MovementComponent> movementCHandle = ent->get<MovementComponent>();
			ComponentHandle<PositionComponent> positionCHandle = ent->get<PositionComponent>();
			glm::vec3 unit_front = glm::normalize(glm::vec3(positionCHandle->Front.x, 0.0f, positionCHandle->Front.z)) * 10.0f;
			glm::vec3 unit_right = glm::normalize(glm::vec3(positionCHandle->Right.x, 0.0f, positionCHandle->Right.z)) * 10.0f;

            float y_velocity = movementCHandle->Velocity.y;
			movementCHandle->Velocity = (float)forward * unit_front - (float)backward * unit_front + (float)right * unit_right - (float)left * unit_right;
            movementCHandle->Velocity += movementCHandle->Velocity * (float)speed_up * 3.0f;
            movementCHandle->Velocity.y = y_velocity;
            
            if (jump) {
                //printf("jump\n");
                jump = false;
                playerCHandle->isJumping = true;
                movementCHandle->Velocity += glm::vec3(0.0, 40.0f, 0.0f);
                movementCHandle->Acceleration = glm::vec3(0.0f, -60.0f, 0.0f);
            }                                                             
		});
	}
};