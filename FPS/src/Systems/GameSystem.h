#pragma once
#include <GLFW/glfw3.h>
#include <iostream>
#include <windows.h>
#include <ECS.h>
#include <Resource.h>
#include <Components/TextComponent.h>
#include <Events/KeyEvents.h>
#include <Events/TextChangeEvent.h>

using namespace ECS;
using namespace std;

const float GAME_TIME = 60.0f;
const float READY_TIME = 2.0f;

class GameSystem : public EntitySystem,
	public EventSubscriber<KeyPressEvent> {
public:

	float time_left;
	Resource::TextureResource textureResource;
	Resource::CubeResource box_resource;

	virtual void configure(class World* world) override
	{
		world->subscribe<KeyPressEvent>(this);
	}

	virtual void unconfigure(class World* world) override
	{
		world->unsubscribeAll(this);
	}

	virtual void receive(class World* world, const KeyPressEvent& event) override {
		auto windowCHandle = world->getSingletonComponent<WindowInfoSingletonComponent>();
		int window_width, window_height;
		glfwGetWindowSize(windowCHandle->Window, &window_width, &window_height);

		// 还没准备，开始等待
		if (!windowCHandle->game_ready) {
			windowCHandle->game_ready = true;
			time_left = READY_TIME;

			// 清掉所有 monster
			world->each<ObjectComponent>([&](Entity* ent, ComponentHandle<ObjectComponent> objectCHandle) -> void {
				if (objectCHandle->id == "monster") {
					ent->removeAll();
				}
			});

			world->each<TextComponent>([&](Entity* ent, ComponentHandle<TextComponent> textCHandle) -> void {
				if (textCHandle->info == "end") {
					ent->removeAll();
				}
			});

			//  重置得分和子弹
			windowCHandle->score = 0;
			world->each<PlayerComponent>([&](Entity* ent, ComponentHandle<PlayerComponent> playerCHandle) -> void {	
				playerCHandle->cur_bullet = playerCHandle->bullet_capacity;
			});

			for (int i = 0; i < 5; i++) {
				Entity* box = world->create();
				if (i == 0) {
					box_resource.init(10.0f, 20.0f, 10.0f, textureResource.container_diffuse, textureResource.container_specular);
					box->assign<ObjectComponent>(box_resource.vertices, box_resource.indices, box_resource.textures, "box0");
					box->assign<PositionComponent>(glm::vec3(0.0f, 10.0f, 0.0f));
					box->assign<CollisionComponent>(10.0f, 30.0f, 10.0f);
				}
				else {
					float x = 0.0f, z = 0.0f;
					if (i == 1) x = 20.0f;
					if (i == 2) x = -20.0f;
					if (i == 3) z = 20.0f;
					if (i == 4) z = -20.0f;

					box_resource.init(10.0f, 10.0f, 10.0f, textureResource.container_diffuse, textureResource.container_specular);
					box->assign<ObjectComponent>(box_resource.vertices, box_resource.indices, box_resource.textures, "guarding_box");
					box->assign<PositionComponent>(glm::vec3(x, 5.0f, z));
					box->assign<CollisionComponent>(10.0f, 10.0f, 10.0f);
				}
			}

			Entity* ready_text = world->create();
			ready_text->assign<TextComponent>("ready", "Ready", 0.4f, 0.45f, 1.5f, window_width, window_height, glm::vec3(0.5, 0.8f, 0.2f), windowCHandle->gameRootPath + "/resources/fonts/");

			return;
		}
	}

	GameSystem(Resource::TextureResource _textureResource, Resource::CubeResource _box_resource) {
		time_left = GAME_TIME;
		textureResource = _textureResource;
		box_resource = _box_resource;
	}

	virtual void tick(class World* world, float deltaTime) override {
		auto windowCHandle = world->getSingletonComponent<WindowInfoSingletonComponent>();
		int window_width, window_height;
		glfwGetWindowSize(windowCHandle->Window, &window_width, &window_height);

		// 等待计时
		if (windowCHandle->game_ready && !windowCHandle->game_start) {
			time_left -= deltaTime;
			if (time_left < 0) {
				windowCHandle->game_start = true;
				time_left = GAME_TIME;

				world->each<TextComponent>([&](Entity* ent, ComponentHandle<TextComponent> textCHandle) -> void {
					if (textCHandle->info == "ready") {
						ent->removeAll();
					}
				});
			}
		}

		if (windowCHandle->game_start) {
			time_left -= deltaTime;
			if (time_left < 0) {
				windowCHandle->game_start = false;
				windowCHandle->game_ready = false;
				Entity* win_text = world->create();
				win_text->assign<TextComponent>("end", "You Win", 0.4f, 0.42f, 1.5f, window_width, window_height, glm::vec3(0.5, 0.8f, 0.2f), windowCHandle->gameRootPath + "/resources/fonts/");
				
				world->each<ObjectComponent>([&](Entity* ent, ComponentHandle<ObjectComponent> objectCHandle) -> void {
					if (objectCHandle->id == "monster") {
						auto positionCHandle = ent->get<PositionComponent>();

						ParticleSystem::simulateDisappearing(world, glm::vec3(positionCHandle->Position.x, 4.0f, positionCHandle->Position.z));

						ent->removeAll();
					}
				});
			}

			world->each<ObjectComponent>([&](Entity* ent, ComponentHandle<ObjectComponent> objectCHandle) -> void {
				/*if (objectCHandle->id == "player") {
					auto positionCHandle = ent->get<PositionComponent>();
					cout << positionCHandle->Position.x << ' ' << positionCHandle->Position.z << endl;
				}*/

				if (objectCHandle->id == "monster") {
					auto positionCHandle = ent->get<PositionComponent>();
					glm::vec3 position = positionCHandle->Position;

					// 碰到箱子会破坏箱子；碰到中间的就输
					if (position.x < 11.0f && position.x > -11.0f && position.z < 11.0f && position.z > -11.0f) {
						windowCHandle->game_start = false;
						windowCHandle->game_ready = false;

						Entity* win_text = world->create();
						win_text->assign<TextComponent>("end", "You Lose", 0.4f, 0.42f, 1.5f, window_width, window_height, glm::vec3(0.5, 0.8f, 0.2f), windowCHandle->gameRootPath + "/resources/fonts/");

						world->each<ObjectComponent>([&](Entity* ent, ComponentHandle<ObjectComponent> objectCHandle) -> void {
							if (objectCHandle->id == "monster") {
								ent->remove<MovementComponent>();
							}
						});
					}
				}
			});
		}

		updateTime(world);
	}

	void updateTime(class World* world) {
		world->each<TextComponent>([&](Entity* ent, ComponentHandle<TextComponent> textCHandle) -> void {
			if (textCHandle->info == "time") {
				string tmp;
				ostringstream osstream;
				osstream << int(time_left) << "s";

				world->emit<TextChangeEvent>({ "time", osstream.str() });
			}
		});
	}
};