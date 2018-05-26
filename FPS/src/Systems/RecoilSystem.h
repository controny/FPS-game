#pragma once
#include <GLFW/glfw3.h>

#include <ECS.h>
#include <Components/CameraInfoSingletonComponent.h>
#include <Components/WindowInfoSingletonComponent.h>
#include <Events/MouseMovementEvent.h>
#include <Events/KeyEvents.h>

using namespace ECS;


// 后坐力系统，更改 camera 仰角（和鼠标移动效果一样）和准心大小
class RecoilSystem : public EntitySystem,
	public EventSubscriber<KeyPressEvent>,
	public EventSubscriber<MousePressEvent> {
public:

	virtual void configure(class World* world) override
	{
		world->subscribe<KeyPressEvent>(this);
		world->subscribe<MousePressEvent>(this);
	}

	virtual void unconfigure(class World* world) override
	{
		world->unsubscribeAll(this);
	}

	// 接收到 WASD 按下的事件后，更新 Camera 的位置
	virtual void receive(class World* world, const KeyPressEvent& event) override
	{	
		// 跳起后改变准心。枪发射子弹弄好之后，准心组件跟枪实体后，再弄这个
	}

	virtual void receive(class World* world, const MousePressEvent& event) override
	{
		// 怎么实现后坐力？跳上去之后还能回来？而且过渡自然？
		/*if (event.key == MOUSE_LEFT)
			world->emit<MouseMovementEvent>({ 0.5f , 1.5f });*/
	}

	RecoilSystem() {
		init();
	}

	void init() {

	}

	virtual void tick(class World* world, float deltaTime) override {
		
	}
};