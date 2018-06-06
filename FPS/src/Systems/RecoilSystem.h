#pragma once
#include <GLFW/glfw3.h>

#include <ECS.h>
#include <Components/WindowInfoSingletonComponent.h>
#include <Events/MouseMovementEvent.h>
#include <Events/KeyEvents.h>
#include <iostream>
#include <ctime>
#include <stdlib.h>

using namespace ECS;


// 后坐力系统，更改 camera 仰角（和鼠标移动效果一样）和准心大小
class RecoilSystem : public EntitySystem,
	public EventSubscriber<KeyPressEvent>,
	public EventSubscriber<MousePressEvent> {
public:
	const float move_x = 3.0f, move_y = 10.0f, t = 0.15f;//move_x,move_y:总前进的偏移值;
	float dx = 0, dy = 0, bx = 0, by = 0;//dx,dy:每一次tick前进的偏移值;bx,by:每一次tick返回的偏移值
	float x = 0, y = 0;
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
	void   Delay(int   time)//time*1000为秒数 
	{
		clock_t   now = clock();

		while (clock() - now   <   time);
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
	
	virtual void receive(class World* world, const MousePressEvent& event) override
	{
		// 怎么实现后坐力？跳上去之后还能回来？而且过渡自然？
		if (event.key == MOUSE_LEFT) {
			dx = move_x;
			dy = move_y;
			bx = -move_x;
			by = -move_y;
		}	
	}

	RecoilSystem() {
		init();
	}

	void init() {

	}
	/*
	virtual void tick(class World* world, float deltaTime) override {
		if (dx != 0) {
			if ((dx > move_x * (deltaTime / t)) && (dy > move_y * (deltaTime / t))) {
				world->emit<MouseMovementEvent>({ move_x * (deltaTime / t) , move_y * (deltaTime / t) });
				dx = dx - (move_x * (deltaTime / t));
				dy = dy - (move_y * (deltaTime / t));
				cout << dx << "abc" << move_x * (deltaTime / t) << endl;
			}
			else {
				world->emit<MouseMovementEvent>({ dx , dy });
				dx = dy = 0;
			}
		}
		else {
			if ((bx < -move_x * (deltaTime / t)) && (by < -move_y * (deltaTime / t))) {
				world->emit<MouseMovementEvent>({ -move_x * (deltaTime / t) , -move_y * (deltaTime / t) });
				bx += (move_x * (deltaTime / t));
				by += (move_y * (deltaTime / t));
			}
			else {
				world->emit<MouseMovementEvent>({ bx , by });
				bx = by = 0;
			}
		}
	}
	*/
	//在dx,dy上加上高斯随机数，实现屏幕抖动
	virtual void tick(class World* world, float deltaTime) override {
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
				//cout << y << "abc" << endl;
			}
			else {
				world->emit<MouseMovementEvent>({ bx , by });
				bx = by = 0;
			}
		}
	}
	
};