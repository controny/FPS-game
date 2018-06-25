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


// ������ϵͳ������ camera ���ǣ�������ƶ�Ч��һ������׼�Ĵ�С
class RecoilSystem : public EntitySystem,
	public EventSubscriber<KeyPressEvent>,
	public EventSubscriber<FireEvent> {
public:
	const float move_x = 3.0f, move_y = 10.0f, t = 0.3f;//move_x,move_y:��ǰ����ƫ��ֵ;
	float dx = 0, dy = 0, bx = 0, by = 0;//dx,dy:ÿһ��tickǰ����ƫ��ֵ;bx,by:ÿһ��tick���ص�ƫ��ֵ
	float x = 0, y = 0, px = 0;
	float oriSize = 0.025f, desSize = 0.08f, dSize = 0.08f, bSize = 0.08f, tempSize = 0.025f;//oriSize:ԭʼ׼�Ĵ�С��desSize:Ŀ��׼�Ĵ�С��dSize,bSize:׼�ı����С�Ħ�ֵ
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

	// ���յ� WASD ���µ��¼��󣬸��� Camera ��λ��
	virtual void receive(class World* world, const KeyPressEvent& event) override
	{	
		// �����ı�׼�ġ�ǹ�����ӵ�Ū��֮��׼�������ǹʵ�����Ū���
	}

	//��ȡ0��1�����
	float GetRand()
	{
		float a = rand() / double(RAND_MAX);
		return a;
	}
	//��˹�����
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
		// ��ôʵ�ֺ�����������ȥ֮���ܻ��������ҹ�����Ȼ��
		
		/*�������ʱ��dt<0�Żᴥ��������¼�*/
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

	//��dx,dy�ϼ��ϸ�˹�������ʵ����Ļ�����Լ�׼�ı仯
	virtual void tick(class World* world, float deltaTime) override {
		/*�������ʱ��dt<0�Żᴥ��������¼�*/
		
		if (deltaTime < 0.07) {
			dt -= deltaTime;
		}
		
		//cout << "yyp" << deltaTime << endl;
		//cout << "abc" << dt << endl;

		/*ȡ���ڴ�С������ȷ��׼��Ϊ������*/
		auto windowCHandle = world->getSingletonComponent<WindowInfoSingletonComponent>();
		int window_width, window_height;
		glfwGetWindowSize(windowCHandle->Window, &window_width, &window_height);
		float window_rate = (float)window_width / (float)window_height;
		
		world->each<PostComponent>([&](Entity* ent, ComponentHandle<PostComponent> postCHandle) -> void {
			/* ��Ļ���� */
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
			/* ׼�Ĵ�С�仯 */
			if (dSize != desSize) {
				if ((dSize < desSize)) {
					postCHandle->setSize(dSize, window_rate);
					postCHandle->init(glm::vec3(0.0f, 1.0f, 0.0f), dSize, window_rate);
					px = getNumberInNormalDistribution(0.006f, 0.001f);
					dSize += px;
					tempSize = dSize;
				}
				else {
					postCHandle->setSize(desSize, window_rate);
					dSize = desSize;
					
				}
			}
			else {
				if ((bSize > oriSize)) {
					bSize -= (desSize * (deltaTime / t));
					postCHandle->setSize(bSize, window_rate);
				}
				else {
					postCHandle->setSize(oriSize, window_rate);
					bSize = oriSize;
					tempSize = 0.025f;
				}
			}
		});
	}
};