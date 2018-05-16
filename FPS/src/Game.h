#pragma once
#include <GLFW/glfw3.h>
#include <iostream>
#include <ECS.h>

#include <Resource.h>
#include <Components/MeshComponent.h>
#include <Components/CameraInfoSingletonComponent.h>
#include <Components/WindowInfoSingletonComponent.h>
#include <Components/LightingInfoSingletonComponent.h>
#include <Systems/RenderSystem/RenderSystem.h>
#include <Systems/CameraMovingSystem/CameraMovingSystem.h>
#include <Systems/KeyPressingSystem.h>
#include <Systems/MouseMovingSystem.h>
#include <Systems/GUISystem.h>


namespace Game {

	World* world = World::createWorld();
	GLFWwindow* window;

	glm::vec3 cameraPos(0.0f, 3.0f, 5.0f);

	void init() {
		// Get the data
		Resource resource = Resource();
		
		// Systems
		world->registerSystem(new RenderSystem());
		world->registerSystem(new GUISystem());
		world->registerSystem(new KeyPressingSystem());
		world->registerSystem(new MouseMovingSystem());
		world->registerSystem(new CameraMovingSystem(cameraPos));

		// Entities
		Entity* singletons = world->create();
		singletons->assign<CameraInfoSingletonComponent>(cameraPos);
		singletons->assign<LightingInfoSingletonComponent>();
		singletons->assign<WindowInfoSingletonComponent>(window);

		Entity* cube1 = world->create();
		Entity* cube2 = world->create();

		// Assign the components to entities
		Resource::CubeResource cube_resource, cube_resource1;
		cube_resource.init(glm::vec3(-1.5f, 0.5f, -1.5f), 1.0f);
		cube1->assign<MeshComponent>(cube_resource.vertices, cube_resource.indices, cube_resource.textures);
		cube_resource1.init(glm::vec3(1.5f, -0.5f, 1.5f), 1.0f);
		cube2->assign<MeshComponent>(cube_resource1.vertices, cube_resource1.indices, cube_resource1.textures);
	}
};


