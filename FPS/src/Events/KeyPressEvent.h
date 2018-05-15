#pragma once

#include <ECS.h>
#include <camera\camera.h>

enum Action {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

struct KeyPressEvent {
	Action action;
};