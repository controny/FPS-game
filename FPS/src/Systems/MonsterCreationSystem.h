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

	const float margin_time = 3.0f;
	float left_time;
	Resource::ModelResource resource;

	MonsterCreationSystem() {}

	MonsterCreationSystem(Resource::ModelResource monster_resource) {
		resource = monster_resource;
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
		left_time = 1.0f;
	}

	virtual void tick(class World* world, float deltaTime) override {
		left_time -= deltaTime;
		if (left_time < 0) {
			auto windowCHandle = world->getSingletonComponent<WindowInfoSingletonComponent>();

			Entity* monster = world->create();

			float x = (rand() - 1) / double(RAND_MAX) * 100;
			float z = (rand() - 1) / double(RAND_MAX) * 100;

			monster->assign<ObjectComponent>(resource.textures_loaded, resource.meshes);
			monster->assign<PositionComponent>(glm::vec3(x, 0.0f, z));
			monster->assign<CollisionComponent>(-2.0f, 2.0f, 0.0f, 4.0f, -1.5f, 1.5f);
			monster->assign<MovementComponent>(glm::normalize(glm::vec3(-x, 0.0f, -z)) * 3.0f, glm::vec3(0.0f, 0.0f, 0.0f));
			monster->assign<HPComponent>();

			left_time = margin_time;
		}
	}
};