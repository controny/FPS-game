#pragma once

#include <ECS.h>

using namespace ECS;

struct MouseMovementEvent {
	ECS_DECLARE_TYPE;

	float xoffset;
	float yoffset;
};

ECS_DEFINE_TYPE(MouseMovementEvent);