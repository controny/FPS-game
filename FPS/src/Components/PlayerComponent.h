#pragma once

#include <ECS.h>
#include <glm/gtc/type_ptr.hpp>

using namespace ECS;

// 有这个 component 的 entity 就是当前控制的玩家；里面放玩家需要的数据（暂时还没有）
struct PlayerComponent {

    bool isJumping;

	PlayerComponent() {
        isJumping = false;
	}
};