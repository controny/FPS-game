#pragma once
#include <GLFW/glfw3.h>

#include <ECS.h>
#include <Events/MouseMovementEvent.h>
#include <Components/WindowInfoSingletonComponent.h>

using namespace ECS;

// 根据鼠标移动改变 camera 视角
class MouseMovingSystem : public EntitySystem {
public:

	bool firstMouse;
	float lastX, lastY;

	MouseMovingSystem() {
		init();
	}

	void init() {
		firstMouse = true;
	}

	// 每次轮询到时，获得当前鼠标的位置，计算出 offset，并 emit 出鼠标移动的事件
	virtual void tick(class World* world, float deltaTime) override {
		double xpos, ypos;

		world->each<WindowInfoSingletonComponent>([&](Entity* ent, ComponentHandle<WindowInfoSingletonComponent> c) -> void {
			int window_width, window_height;
			glfwGetWindowSize(c->Window, &window_width, &window_height);
			glfwGetCursorPos(c->Window, &xpos, &ypos);

			if (firstMouse) {
				lastX =(float)window_width / 2.0f;
				lastY = (float)window_height / 2.0f;
			}
		});
		
		if (firstMouse) {
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;

		lastX = xpos;
		lastY = ypos;

		world->emit<MouseMovementEvent>({ xoffset ,yoffset });
	}
};