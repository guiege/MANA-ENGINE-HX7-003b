#pragma once

#include "Character.h"

class TestCharacter : public Character
{
public:

	enum State
	{
		idle = 0, atk
	};

	TestCharacter(InputHandler* handler, Texture& texture, const char* file, const float xpos, const float ypos, const float width, const float height, const float rot, std::vector<Solid>& solids, glm::vec4 color = glm::vec4(1.0f))
	:Character(handler, texture, file, xpos, ypos, width, height, rot, solids, color) {}

	void start();

	void update(int tick);

	void enterState(int newstate, int oldstate);

    void exitState(int oldstate, int newstate);

private:
};