#pragma once

#include <ECS.h>

using namespace ECS;

struct TextChangeEvent {
	std::string text_info;
	std::string text;

	TextChangeEvent(std::string _info, std::string _text) : text_info(_info), text(_text) {}
};