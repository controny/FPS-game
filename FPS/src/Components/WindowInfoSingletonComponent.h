#pragma once

#include <ECS.h>

using namespace ECS;

// 单例组件；存储全局的渲染窗口的信息
struct WindowInfoSingletonComponent {
	ECS_DECLARE_TYPE;

	//int Width;
	//int Height;
	GLFWwindow* Window;
	bool showGUI;


	WindowInfoSingletonComponent(GLFWwindow *_window) {
		//Width = _width;
		//Height = _height;
		Window = _window;
		showGUI = false;
	}
};

ECS_DEFINE_TYPE(WindowInfoSingletonComponent);