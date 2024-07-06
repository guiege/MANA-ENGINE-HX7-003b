#pragma once

#include "Character.h"

class TestCharacter : public Character
{
public:

	TestCharacter(Texture& texture, const char* file, const float xpos, const float ypos, const float width, const float height, const float rot, std::vector<Solid>& solids, glm::vec4 color = glm::vec4(1.0f))
	:Character(texture, file, xpos, ypos, width, height, rot, solids, color) {}

	void update(int tick)
	{
		// std::cout << "testing 123" << std::endl;
	}
};