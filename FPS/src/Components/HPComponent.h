#pragma once
#include <GLFW/glfw3.h>


struct HPComponent {
	
	int HP;

	HPComponent() {
		init();
	}

	void init() {
		HP = 100;
	}
};