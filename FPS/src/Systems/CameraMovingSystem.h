#pragma once

#pragma once
#include <glad/glad.h>

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Shader.h>
#include <Camera.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <ECS.h>
#include <Components/MeshComponent.h>
#include <Components/CameraInfoSingletonComponent.h>
#include <Components/WindowInfoSingletonComponent.h>
#include <Components/LightingInfoSingletonComponent.h>

using namespace ECS;

struct MouseMovementEvent {
	ECS_DECLARE_TYPE;

	float xoffset;
	float yoffset;
};

ECS_DEFINE_TYPE(MouseMovementEvent);

// 更新 camera 的位置和仰角等信息，并负责更新 pos 和 viewmatrix 的 component 信息
class CameraMovingSystem : public EntitySystem, public EventSubscriber<MouseMovementEvent> {
public:

	Camera camera;
	GLFWwindow* window;

	virtual void configure(class World* world) override
	{
		world->subscribe<MouseMovementEvent>(this);
	}

	virtual void unconfigure(class World* world) override
	{
		world->unsubscribeAll(this);
	}

	virtual void receive(class World* world, const MouseMovementEvent& event) override
	{
		camera.ProcessMouseMovement(event.xoffset, event.yoffset);
	}

	CameraMovingSystem(glm::vec3 _cameraPosition) {
		camera.Position = _cameraPosition;
	}

	void init() {
		
	}

	virtual void tick(class World* world, float deltaTime) override {
		world->each<WindowInfoSingletonComponent>([&](Entity* ent, ComponentHandle<WindowInfoSingletonComponent> c) -> void {
			window = c->Window;
		});
		
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.ProcessKeyboard(FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.ProcessKeyboard(BACKWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.ProcessKeyboard(LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.ProcessKeyboard(RIGHT, deltaTime);

		world->each<CameraInfoSingletonComponent>([&](Entity* ent, ComponentHandle<CameraInfoSingletonComponent> c) -> void {
			c->CameraViewMatrix = camera.GetViewMatrix();
			c->CameraPos = camera.Position;
		});
	}
};