#pragma once

#include <ECS.h>

using namespace ECS;

// 单例组件；存储全局的渲染窗口的信息
struct WindowInfoSingletonComponent {

	GLFWwindow* Window;
	bool showGUI;
	int score;
	bool game_start;
	string gameRootPath;

	WindowInfoSingletonComponent(GLFWwindow *_window, string _gameRootPath) {

		Window = _window;
		showGUI = false;
		score = 0;
		game_start = false;
		gameRootPath = _gameRootPath;
	}
};
