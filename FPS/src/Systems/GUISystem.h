#pragma once
#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>

#include <ECS.h>
#include <Components/WindowInfoSingletonComponent.h>
#include <Events/KeyEvents.h>

using namespace ECS;

// 管理 ImGUI 的显示
class GUISystem : public EntitySystem,
	public EventSubscriber<KeyPressEvent> {
public:

	bool initialized;

	GUISystem() {
		init();
	}

	void init() {
		initialized = false;
	}

	virtual void configure(class World* world) override
	{
		world->subscribe<KeyPressEvent>(this);
	}

	virtual void unconfigure(class World* world) override
	{
		world->unsubscribeAll(this);
	}

	// 接收到 ESC 按下的事件后，显示 GUI
	virtual void receive(class World* world, const KeyPressEvent& event) override
	{
		if (event.key == ESC) {
			auto windowCHandle = world->getSingletonComponent<WindowInfoSingletonComponent>();
			windowCHandle->showGUI = true;
			glfwSetInputMode(windowCHandle->Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}


	// 如果 ImGui 还没有初始化和绑定到 window，先初始化和绑定
	virtual void tick(class World* world, float deltaTime) override {
		auto windowCHandle = world->getSingletonComponent<WindowInfoSingletonComponent>();

		if (!initialized) {
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			ImGui_ImplGlfwGL3_Init(windowCHandle->Window, true);

			initialized = true;
		}


		auto lightCHandle = world->getSingletonComponent<LightingInfoSingletonComponent>();

		//float x = 0.0f, y = 0.0f, z;
		//float cx, cy, cz;
		//float rx, ry;
		//float s;
		//world->each<ObjectComponent, PositionComponent, PlayerComponent, TransformComponent>(
		//	[&](Entity* ent,
		//		ComponentHandle<ObjectComponent> objectCHandle,
		//		ComponentHandle<PositionComponent> positionCHandle,
		//		ComponentHandle<PlayerComponent> playerCHandle,
		//		ComponentHandle<TransformComponent> transformCHandle) -> void {
		//	x = transformCHandle->translate.x;
		//	y = transformCHandle->translate.y;
		//	z = transformCHandle->translate.z;
		//	s = transformCHandle->scale.x;

		//	auto cameraCHandle = ent->get<CameraComponent>();
		//	cx = cameraCHandle->Relative_position.x;
		//	cy = cameraCHandle->Relative_position.y;
		//	cz = cameraCHandle->Relative_position.z;



		//	rx = transformCHandle->rotate_x;
		//	ry = transformCHandle->rotate_y;

		//});

		if (windowCHandle->showGUI) {
			ImGui_ImplGlfwGL3_NewFrame();
			{
				ImGui::Begin("Game Menu", &(windowCHandle->showGUI));  // 用右上角的交叉关闭 GUI

				ImGui::Checkbox("Shadow", &(lightCHandle->shadow_enable));

				ImGui::Text("Shadow type:");
				ImGui::RadioButton("Linear", &(lightCHandle->shadow_type), 0);
				ImGui::RadioButton("PCSS (fixed BS region)", &(lightCHandle->shadow_type), 1);
				ImGui::RadioButton("PCSS", &(lightCHandle->shadow_type), 2);

				ImGui::Text("\nLighting parameters:");
				ImGui::SliderFloat("Ambient strength", &(lightCHandle->AmbientStrength), 0.0f, 1.0f);
				ImGui::SliderFloat("Specular strength", &(lightCHandle->SpecularStrength), 0.0f, 1.0f);
				ImGui::SliderFloat("Diffuse strength", &(lightCHandle->DiffuseStrength), 0.0f, 1.0f);
				ImGui::SliderFloat("Shininess", &(lightCHandle->Shininess), 0.1f, 64.0f);
                ImGui::Text("Light Source Position");
                ImGui::SliderFloat("x", &lightCHandle->LightPos.x, -100.0f, 100.0f);
                ImGui::SliderFloat("y", &lightCHandle->LightPos.y, -100.0f, 100.0f);
                ImGui::SliderFloat("z", &lightCHandle->LightPos.z, -100.0f, 100.0f);

				//ImGui::SliderFloat("x", &x, -20.0f, 20.0f);
				//ImGui::SliderFloat("y", &y, -20.0f, 20.0f);
				//ImGui::SliderFloat("z", &z, -20.0f, 20.0f);
				//ImGui::SliderFloat("s", &s, 0.05f, 0.001f);
				//ImGui::SliderFloat("cx", &cx, -20.0f, 20.0f);
				//ImGui::SliderFloat("cy", &cy, -20.0f, 20.0f);
				//ImGui::SliderFloat("cz", &cz, -20.0f, 20.0f);
				//ImGui::SliderFloat("rx", &rx, -180.0f, 180.0f);
				//ImGui::SliderFloat("ry", &ry, -180.0f, 180.0f);
				ImGui::End();
			}

			ImGui::Render();
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
		} else {
			glfwSetInputMode(windowCHandle->Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}

		/*world->each<ObjectComponent, PositionComponent, PlayerComponent, TransformComponent>(
			[&](Entity* ent,
				ComponentHandle<ObjectComponent> objectCHandle,
				ComponentHandle<PositionComponent> positionCHandle,
				ComponentHandle<PlayerComponent> playerCHandle,
				ComponentHandle<TransformComponent> transformCHandle) -> void {
			transformCHandle->translate.x = x;
			transformCHandle->translate.y = y;
			transformCHandle->translate.z = z;
			transformCHandle->scale = glm::vec3(s, s, s);

			auto cameraCHandle = ent->get<CameraComponent>();
			cameraCHandle->Relative_position.x = cx;
			cameraCHandle->Relative_position.y = cy;
			cameraCHandle->Relative_position.z = cz;

			transformCHandle->rotate_x = rx;
			transformCHandle->rotate_y = ry;

		});*/
	}
};