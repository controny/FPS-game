#pragma once
#include <GLFW/glfw3.h>

#include <ECS.h>
#include <Components/WindowInfoSingletonComponent.h>
#include <Events/MouseMovementEvent.h>
#include <Events/KeyEvents.h>
#include <Events/FireEvent.h>
#include <iostream>
#include <ctime>
#include <stdlib.h>
#include <Components/PostComponent.h>

using namespace ECS;


// 后坐力系统，更改 camera 仰角（和鼠标移动效果一样）和准心大小
class RecoilSystem : public EntitySystem,
	public EventSubscriber<KeyPressEvent>,
	public EventSubscriber<FireEvent> {
public:
	const float move_x = 3.0f, move_y = 10.0f, t = 0.3f;//move_x,move_y:总前进的偏移值;
	float dx = 0, dy = 0, bx = 0, by = 0;//dx,dy:每一次tick前进的偏移值;bx,by:每一次tick返回的偏移值
	float x = 0, y = 0, px = 0;
	float oriSize = 0.025f, desSize = 0.08f, dSize = 0.08f, bSize = 0.08f, tempSize = 0.025f;//oriSize:原始准心大小，desSize:目标准心大小；dSize,bSize:准心变大或变小的Δ值
	const float T = 0.1f;
	float dt = 0.15f;
	virtual void configure(class World* world) override
	{
		world->subscribe<KeyPressEvent>(this);
		world->subscribe<FireEvent>(this);
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

	//获取0—1随机数
	float GetRand()
	{
		float a = rand() / double(RAND_MAX);
		return a;
	}
	//高斯随机数
	float uniform2NormalDistribution() {

		float sum = 0.0;
		for (int i = 0; i<12; i++) {
			sum = sum + GetRand();
		}
		return sum - 6.0;
	}
	float getNumberInNormalDistribution(float mean, float std_dev) {
		return mean + (uniform2NormalDistribution()*std_dev);
	}
	virtual void receive(class World* world, const FireEvent& event) override
	{
		// 怎么实现后坐力？跳上去之后还能回来？而且过渡自然？
		
		/*连续点击时当dt<0才会触发鼠标点击事件*/
		world->each<PostComponent>([&](Entity* ent, ComponentHandle<PostComponent> postCHandle) -> void {
			dx = move_x;
			dy = move_y;
			bx = -move_x;
			by = -move_y;

			dSize = tempSize;
			bSize = desSize;

			dt = 0.15f;
		});
		
	}

	RecoilSystem() {
		init();
	}

	void init() {

	}

	//在dx,dy上加上高斯随机数，实现屏幕抖动以及准心变化
	virtual void tick(class World* world, float deltaTime) override {
		/*连续点击时当dt<0才会触发鼠标点击事件*/
		
		if (deltaTime < 0.07) {
			dt -= deltaTime;
		}
		
		//cout << "yyp" << deltaTime << endl;
		//cout << "abc" << dt << endl;

		/*取窗口大小比例，确保准心为正方形*/
		auto windowCHandle = world->getSingletonComponent<WindowInfoSingletonComponent>();
		int window_width, window_height;
		glfwGetWindowSize(windowCHandle->Window, &window_width, &window_height);
		float window_rate = (float)window_width / (float)window_height;
		
		world->each<PostComponent>([&](Entity* ent, ComponentHandle<PostComponent> postCHandle) -> void {
			/* 屏幕抖动 */
			if (dx != 0) {
				x = getNumberInNormalDistribution(0.5f, 0.8f);
				y = getNumberInNormalDistribution(1.1f, 0.3f);
				if ((dx > x) && (dy > y)) {
					world->emit<MouseMovementEvent>({ x , y });
					dx = dx - x;
					dy = dy - y;
				}
				else {
					world->emit<MouseMovementEvent>({ dx , dy });
					dx = dy = 0;
				}
			}
			else {
				x = -getNumberInNormalDistribution(0.5f, 0.8f);
				y = -getNumberInNormalDistribution(1.5f, 0.3f);
				if ((bx < x) && (by < y)) {
					world->emit<MouseMovementEvent>({ x , y });
					bx += (-x);
					by += (-y);
				}
				else {
					world->emit<MouseMovementEvent>({ bx , by });
					bx = by = 0;
				}
			}
			/* 准心大小变化 */
			if (dSize != desSize) {
				if ((dSize < desSize)) {
					postCHandle->init(glm::vec3(0.0f, 1.0f, 0.0f), dSize, window_rate);
					px = getNumberInNormalDistribution(0.006f, 0.001f);
					dSize += px;
					tempSize = dSize;
				}
				else {
					postCHandle->init(glm::vec3(0.0f, 1.0f, 0.0f), desSize, window_rate);
					dSize = desSize;
					
				}
			}
			else {
				if ((bSize > oriSize)) {
					bSize -= (desSize * (deltaTime / t));
					postCHandle->init(glm::vec3(0.0f, 1.0f, 0.0f), bSize, window_rate);
				}
				else {
					postCHandle->init(glm::vec3(0.0f, 1.0f, 0.0f), oriSize, window_rate);
					bSize = oriSize;
					tempSize = 0.025f;
				}
			}
		});
	}
};