#pragma once
#include <GLFW/glfw3.h>
#include <iostream>
#include <ECS.h>
#include <Components/TextComponent.h>
#include <Events/TextChangeEvent.h>

using namespace ECS;
using namespace std;


class TextSystem : public EntitySystem,
	public EventSubscriber<TextChangeEvent> {
public:

	float last_window_width, last_window_height;

	virtual void configure(class World* world) override
	{
		world->subscribe<TextChangeEvent>(this);
	}

	virtual void unconfigure(class World* world) override
	{
		world->unsubscribeAll(this);
	}

	virtual void receive(class World* world, const TextChangeEvent& event) override {
		world->each<TextComponent>([&](Entity* ent, ComponentHandle<TextComponent> textCHandle) -> void {
			if (textCHandle->info == event.text_info) {
				textCHandle->setText(event.text);
				return;
			}
		});
	}

	void init() {
		last_window_width = last_window_height = 0;
	}

	virtual void tick(class World* world, float deltaTime) override {
		// change the text pos via window size
		auto windowCHandle = world->getSingletonComponent<WindowInfoSingletonComponent>();

		int window_width, window_height;
		glfwGetWindowSize(windowCHandle->Window, &window_width, &window_height);
		
		if (last_window_width != window_width || last_window_height != window_height) {
			world->each<TextComponent>([&](Entity* ent, ComponentHandle<TextComponent> textCHandle) -> void {
				textCHandle->setPos(window_width, window_height);
			});

			last_window_width = window_width;
			last_window_height = window_height;
		}
	}
};