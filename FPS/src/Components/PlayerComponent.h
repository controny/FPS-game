#pragma once

#include <ECS.h>
#include <glm/gtc/type_ptr.hpp>

using namespace ECS;

// 有这个 component 的 entity 就是当前控制的玩家；里面放玩家需要的数据（暂时还没有）
struct PlayerComponent {

    bool isJumping;
	bool can_shot;  // 现在是否能射击
	int bullet_capacity, cur_bullet;  // 弹夹容量

	PlayerComponent() {
		init();
        
	}

	void init() {
		bullet_capacity = 30;
		cur_bullet = bullet_capacity;
		isJumping = false;
	}
};