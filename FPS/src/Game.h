#pragma once
#include <GLFW/glfw3.h>
#include <iostream>
#include <ECS.h>

#include <Resource.h>
#include <Components/MovementComponent.h>
#include <Components/PositionComponent.h>
#include <Components/TextComponent.h>
#include <Components/ObjectComponent.h>
#include <Components/TextComponent.h>
#include <Components/CameraInfoSingletonComponent.h>
#include <Components/WindowInfoSingletonComponent.h>
#include <Components/LightingInfoSingletonComponent.h>
#include <Components/SkyboxInfoSingletonComponent.h>
#include <Components/PostComponent.h>
#include <Systems/RenderSystem/RenderSystem.h>
#include <Systems/CameraMovingSystem/CameraMovingSystem.h>
#include <Systems/KeyPressingSystem.h>
#include <Systems/MouseMovingSystem.h>
#include <Systems/GUISystem.h>
#include <Systems/MovementSystem.h>
#include <Systems/RecoilSystem.h>


namespace Game {

	World* world = World::createWorld();
	GLFWwindow* window;

	glm::vec3 cameraPos(0.0f, 3.0f, 5.0f);

	void init() {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        
        // Get the data
		Resource resource = Resource();
		Resource::SkyBoxResource skybox_resource;
		skybox_resource.init();
		
		// Systems
		world->registerSystem(new GUISystem());
		world->registerSystem(new KeyPressingSystem());
		world->registerSystem(new MouseMovingSystem());
		world->registerSystem(new CameraMovingSystem(cameraPos));
		//world->registerSystem(new RecoilSystem());
        world->registerSystem(new MovementSystem());
		world->registerSystem(new RenderSystem());


		// Singleton components
		world->createSingletonComponent<CameraInfoSingletonComponent>(cameraPos);
		world->createSingletonComponent<LightingInfoSingletonComponent>();
		world->createSingletonComponent<WindowInfoSingletonComponent>(window);
		world->createSingletonComponent<SkyboxInfoSingletonComponent>(skybox_resource.vertices, skybox_resource.indices, skybox_resource.textures);

		// Entities
		Entity* wall = world->create();
		Entity* ourModel = world->create();
		Entity* ground = world->create();
		Entity* text = world->create();
		Entity* test_post = world->create();  // 以后 post 赋给 gun 的 entity，现在只是测试
		Entity* particles = world->create();

		// Load texture resource
		Resource::TextureResource textureResource;
		textureResource.init();

		// Assign the components to entities
		Resource::CubeResource wall_resource, ground_resource;
		wall_resource.init(10.0f, 2.0f, 30.0f, textureResource.container_diffuse, textureResource.container_specular);
		wall->assign<ObjectComponent>(wall_resource.vertices, wall_resource.indices, wall_resource.textures);
        wall->assign<PositionComponent>(glm::vec3(10.0f, 0.0f, 0.0f));
        wall->assign<MovementComponent>(glm::vec3(5.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));

		ground_resource.init(500.0f, 500.0f, 1.0f, textureResource.ground_diffuse, textureResource.ground_specular);
		ground->assign<ObjectComponent>(ground_resource.vertices, ground_resource.indices, ground_resource.textures);
        ground->assign<PositionComponent>(glm::vec3(0.0f, 0.0f, 0.0f));

		ourModel->assign<ObjectComponent>("resources/objects/nanosuit/nanosuit.obj");
        ourModel->assign<PositionComponent>(glm::vec3(0.0f, 0.0f, 0.0f));
        ourModel->assign<MovementComponent>(glm::vec3(5.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));

		text->assign<TextComponent>("test", 1.0f, 1.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));

		test_post->assign<PostComponent>(glm::vec3(0.0f, 1.0f, 0.0f), 0.025f);

		particles->assign<ParticleComponent>(5000, glm::vec3(0.0f, 3.0f, -10.0f), 5.0f, glm::vec3(0.0f, 8.0f, 0.0f), 128, 1, 1);
	}
};


