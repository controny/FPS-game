#pragma once
#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>

#include <ECS.h>
#include <Components/WindowInfoSingletonComponent.h>
#include <Events/KeyPressEvent.h>

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
			world->each<WindowInfoSingletonComponent>([&](Entity* ent, ComponentHandle<WindowInfoSingletonComponent> c) -> void {
				c->showGUI = true;
			});
		}
	}


	// 如果 ImGui 还没有初始化和绑定到 window，先初始化和绑定
	virtual void tick(class World* world, float deltaTime) override {
		
		world->each<WindowInfoSingletonComponent>([&](Entity* ent, ComponentHandle<WindowInfoSingletonComponent> c) -> void {
			if (!initialized) {
				ImGui::CreateContext();
				ImGuiIO& io = ImGui::GetIO(); (void)io;
				ImGui_ImplGlfwGL3_Init(c->Window, true);

				initialized = true;
			}

			if (c->showGUI) {
				ImGui_ImplGlfwGL3_NewFrame();
				{
					ImGui::Begin("Game Menu", &(c->showGUI));  // 用右上角的交叉关闭 GUI
					ImGui::Text("dfdfdf");

					ImGui::Text("hhhhhh");
					ImGui::End();
				}

				ImGui::Render();
				ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
			}
		});	
	}
};