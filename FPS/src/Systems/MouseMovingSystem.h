#pragma once
#include <GLFW/glfw3.h>

#include <ECS.h>
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
		auto windowCHandle = world->getSingletonComponent<WindowInfoSingletonComponent>();
		
		if (windowCHandle->showGUI) {
			firstMouse = true;
			return;
		}

		double xpos, ypos;
		int window_width, window_height;
		glfwGetWindowSize(windowCHandle->Window, &window_width, &window_height);
		glfwGetCursorPos(windowCHandle->Window, &xpos, &ypos);

		if (firstMouse) {
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;

		lastX = xpos;
		lastY = ypos;

		// 更新 camera 的数据（之前 camera 的 updateVectors 方法和 processMouseMovement 方法
		world->each<PlayerComponent>([&](Entity* ent, ComponentHandle<PlayerComponent> playerCHandle) -> void {
			auto cameraCHandle = ent->get<CameraComponent>();
			auto positionCHandle = ent->get<PositionComponent>();
            auto collisionCHandle = ent->get<CollisionComponent>();

			xoffset *= cameraCHandle->MouseSensitivity;
			yoffset *= cameraCHandle->MouseSensitivity;

			cameraCHandle->Yaw += xoffset;
			cameraCHandle->Pitch += yoffset;

			// Make sure that when pitch is out of bounds, screen doesn't get flipped
			if (cameraCHandle->Pitch > 89.0f)
				cameraCHandle->Pitch = 89.0f;
			if (cameraCHandle->Pitch < -89.0f)
				cameraCHandle->Pitch = -89.0f;

			glm::vec3 front;
			front.x = cos(glm::radians(cameraCHandle->Yaw)) * cos(glm::radians(cameraCHandle->Pitch));
			front.y = sin(glm::radians(cameraCHandle->Pitch));
			front.z = sin(glm::radians(cameraCHandle->Yaw)) * cos(glm::radians(cameraCHandle->Pitch));
            
			positionCHandle->Front = glm::normalize(front);

            // unfinished
            //float x = positionCHandle->Front.x;
            //float z = positionCHandle->Front.z;
            //float angle;				
            //if (x > 0 && z > 0) {            
            //    angle = fmod(glm::acos(z), 1.57);              
            //}
            //else if (x > 0 && z < 0) {
            //    angle = fmod(float(glm::acos(z)), 1.57);
            //}
            //else if (x < 0 && z > 0) {
            //    angle = fmod(float(-glm::acos(z)), 1.57);
            //}
            //else if (x < 0 && z < 0) {
            //    angle = fmod(float(6.28 - glm::acos(z)), 1.57);
            //}
            //// update collision body
            ////x = xcosθ + zsinθ
            ////z = xsinθ + zcosθ
            //float original_width = collisionCHandle->original_width;
            //float original_depth = collisionCHandle->original_depth;
            //float new_width = original_width * cos(angle) + original_depth * sin(angle);
            //float new_depth = original_width * sin(angle) + original_depth * cos(angle);
            //collisionCHandle->updateBody(new_width, new_depth);
            //printf("angle: %f\n", angle);
            //collisionCHandle->Print();

			positionCHandle->Right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
			positionCHandle->Up = glm::normalize(glm::cross(positionCHandle->Right, front));
		});
	}
};