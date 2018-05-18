#pragma once
#include <GLFW/glfw3.h>

#include "Camera.h"
#include <ECS.h>
#include <Components/ObjectComponent.h>
#include <Components/CameraInfoSingletonComponent.h>
#include <Components/WindowInfoSingletonComponent.h>
#include <Components/LightingInfoSingletonComponent.h>
#include <Events/MouseMovementEvent.h>
#include <Events/KeyPressEvent.h>

using namespace ECS;

// 更新 camera 的位置和仰角等信息，并负责更新 pos 和 viewmatrix 的 component 信息
class CameraMovingSystem : public EntitySystem, 
	public EventSubscriber<MouseMovementEvent>,
	public EventSubscriber<KeyPressEvent> {
public:

	Camera camera;

	virtual void configure(class World* world) override
	{
		// 记得注册需要接受的事件！
		world->subscribe<MouseMovementEvent>(this);
		world->subscribe<KeyPressEvent>(this);
	}

	virtual void unconfigure(class World* world) override
	{
		world->unsubscribeAll(this);
	}

	// 接收到鼠标移动的事件后，更新鼠标的位置
	virtual void receive(class World* world, const MouseMovementEvent& event) override
	{
		camera.ProcessMouseMovement(event.xoffset, event.yoffset);
	}

	// 接收到 WASD 按下的事件后，更新 Camera 的位置
	virtual void receive(class World* world, const KeyPressEvent& event) override
	{
		if (event.key == W)
			camera.ProcessKeyboard(FORWARD, event.deltaTime);
		if (event.key == S)
			camera.ProcessKeyboard(BACKWARD, event.deltaTime);
		if (event.key == A)
			camera.ProcessKeyboard(LEFT, event.deltaTime);
		if (event.key == D)
			camera.ProcessKeyboard(RIGHT, event.deltaTime);
	}

	CameraMovingSystem(glm::vec3 _cameraPosition) {
		camera.Position = _cameraPosition;
	}

	void init() {
		
	}

	// 每次轮询到时，就把最新的 camera 位置和视角更新
	virtual void tick(class World* world, float deltaTime) override {
		ComponentHandle<CameraInfoSingletonComponent> cameraCHandle = world->getSingletonComponent<CameraInfoSingletonComponent>();

		cameraCHandle->CameraViewMatrix = camera.GetViewMatrix();
		cameraCHandle->CameraPos = camera.Position;
	}
};