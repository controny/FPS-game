#pragma once
#include <GLFW/glfw3.h>

#include <ECS.h>
#include <Events/KeyPressEvent.h>
#include <Components/WindowInfoSingletonComponent.h>
#include <iostream>
using namespace ECS;

// 检测键盘哪个键被按下
class KeyPressingSystem : public EntitySystem {
public:

	// 每次轮询到时，检测当前按下的按钮，并且 emit 出对应按钮的事件
	virtual void tick(class World* world, float deltaTime) override {
		world->each<WindowInfoSingletonComponent>([&](Entity* ent, ComponentHandle<WindowInfoSingletonComponent> c) -> void {
			if (glfwGetKey(c->Window, GLFW_KEY_W) == GLFW_PRESS)
				world->emit<KeyPressEvent>({ W, deltaTime });
			if (glfwGetKey(c->Window, GLFW_KEY_S) == GLFW_PRESS)
				world->emit<KeyPressEvent>({ S, deltaTime });
			if (glfwGetKey(c->Window, GLFW_KEY_A) == GLFW_PRESS)
				world->emit<KeyPressEvent>({ A, deltaTime });
			if (glfwGetKey(c->Window, GLFW_KEY_D) == GLFW_PRESS)
				world->emit<KeyPressEvent>({ D, deltaTime });
			if (glfwGetKey(c->Window, GLFW_KEY_D) == GLFW_PRESS)
				world->emit<KeyPressEvent>({ ESC,deltaTime });
		});
	}
};