#pragma once

#include <ECS.h>

// 单例组件；存储全局的渲染窗口的信息
struct WindowInfoSingletonComponent {
	ECS_DECLARE_TYPE;

	unsigned int Width;
	unsigned int Height;
	GLFWwindow* Window;


	WindowInfoSingletonComponent(unsigned int _width, unsigned int _height, GLFWwindow *_window) {
		Width = _width;
		Height = _height;
		Window = _window;
	}
};

ECS_DEFINE_TYPE(WindowInfoSingletonComponent);