#ifndef GAME_STATE
#define GAME_STATE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "ResourceManager.h"
#include "Renderer.h"

class GameState
{
	public:
		bool doneLoading;
		bool	Keys[1024];
		bool leftMouse, rightMouse, middleMouse;
		double mouseX, mouseY;
		int tick = 0;
		//State Transitions
		virtual bool enter() = 0;
		virtual bool exit() = 0;

		//Main loop functions
		virtual void update(float dt) = 0;
		virtual void render() = 0;

		//Make sure to call child destructors
		virtual ~GameState() = default;
};

#endif