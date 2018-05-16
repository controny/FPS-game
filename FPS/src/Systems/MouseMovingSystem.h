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

	// 每次轮询到时，如果 GUI 没有在显示，就获得当前鼠标的位置，计算出 offset，并 emit 出鼠标移动的事件
	virtual void tick(class World* world, float deltaTime) override {
		double xpos, ypos;

		world->each<WindowInfoSingletonComponent>([&](Entity* ent, ComponentHandle<WindowInfoSingletonComponent> c) -> void {
			if (c->showGUI) {
				firstMouse = true;
				return;
			}
			int window_width, window_height;
			glfwGetWindowSize(c->Window, &window_width, &window_height);
			glfwGetCursorPos(c->Window, &xpos, &ypos);

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
		});
		
		
	}
};