#pragma once
#include <GLFW/glfw3.h>

#include <ECS.h>
#include <Resource.h>
#include <Components/CollisionComponent.h>
#include <Components/PositionComponent.h>
#include <Components/ObjectComponent.h>
#include <Components/WindowInfoSingletonComponent.h>
#include <Components/HPComponent.h>
#include <Events/KeyEvents.h>
#include <Events/FireEvent.h>
#include <Events/TextChangeEvent.h>

using namespace ECS;

// ÅªÊµÀý»¯äÖÈ¾
class MonsterCreationSystem : public EntitySystem,
	public EventSubscriber<KeyPressEvent> {
public:

	const float margin_time = 1.0f;
	float left_time;
	float square, away;

	Resource::ModelResource resource;

	MonsterCreationSystem() {}

	MonsterCreationSystem(Resource::ModelResource monster_resource, float _square, float _away) {
		resource = monster_resource;
		square = _square;
		away = _away;
		init();
	}

	virtual void configure(class World* world) override
	{
		world->subscribe<KeyPressEvent>(this);
	}

	virtual void unconfigure(class World* world) override
	{
		world->unsubscribeAll(this);
	}

	virtual void receive(class World* world, const KeyPressEvent& event) override {

	}

	void init() {
		left_time = margin_time;
	}

	virtual void tick(class World* world, float deltaTime) override {
		auto windowCHandle = world->getSingletonComponent<WindowInfoSingletonComponent>();

		if (!windowCHandle->game_start) {
			return;
		}

		left_time -= deltaTime;
		if (left_time < 0) {
			Entity* monster = world->create();

			float area = rand() / double(RAND_MAX) * 4;
			float pos = (rand() - 0.5) / double(RAND_MAX) * 2 * (square - 10);
			float velocity = max(rand() / float(RAND_MAX) * 20, 15.0f);
			
			float x = 0.0f, z = 0.0f;
			if (area >= 0 && area < 1) {
				x = away; z = pos;
			}
			if (area >= 1 && area < 2) {
				x = -away; z = pos;
			}
			if (area >= 2 && area < 3) {
				x = pos; z = away;
			}
			if (area >= 3 && area <= 4) {
				x = pos; z = -away;
			}

			float rotate_y;
			glm::vec3 normal = glm::normalize(glm::vec3(x, 0.0f, z));
			if (normal.x > 0 && normal.z > 0) {
				rotate_y = -(1.57 + acos(normal.x));
			}
			if (normal.x > 0 && normal.z < 0) {
				rotate_y = -asin(normal.x);
			}
			if (normal.x < 0 && normal.z > 0) {
				rotate_y = 1.57 + asin(normal.z);
			}
			if (normal.x < 0 && normal.z < 0) {
				rotate_y = acos(-normal.z);
			}

			monster->assign<ObjectComponent>(resource.textures_loaded, resource.meshes, "monster");
			monster->assign<PositionComponent>(glm::vec3(x, 0.0f, z));
			
			monster->assign<CollisionComponent>(-4.0f, 4.0f, 0.0f, 14.0f, -3.0f, 3.0f);
			monster->assign<TransformComponent>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(2.0f, 2.0f, 2.0f), 0.0f, rotate_y);

			monster->assign<MovementComponent>(glm::normalize(glm::vec3(-x, 0.0f, -z)) * velocity, glm::vec3(0.0f, 0.0f, 0.0f));
			monster->assign<HPComponent>();

			left_time = margin_time;
		}
	}
};