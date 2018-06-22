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

	}

	virtual void tick(class World* world, float deltaTime) override {

	}
};