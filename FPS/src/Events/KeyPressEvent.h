#pragma once

#include <ECS.h>
#include <Camera.h>

enum Action {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

struct KeyPressEvent {
	Action action;
};