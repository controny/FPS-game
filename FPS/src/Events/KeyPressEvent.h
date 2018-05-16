#pragma once

#include <ECS.h>

using namespace ECS;

enum Key {
	W,
	S,
	A,
	D,
	ESC
};

struct KeyPressEvent {
	ECS_DECLARE_TYPE;

	KeyPressEvent(Key _key, float _deltaTime)
		: key(_key), deltaTime(_deltaTime) {};
	
	Key key;
	float deltaTime;
};

ECS_DEFINE_TYPE(KeyPressEvent);