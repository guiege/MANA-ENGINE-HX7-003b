#ifndef _SCENE_H_
#define _SCENE_H_

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "ResourceManager.h"
#include "Renderer.h"

class Scene
{
	public:
		bool doneLoading;
		bool paused = false;
		bool advanceFrame = false;
		bool	Keys[1024];
		bool leftMouse, rightMouse, middleMouse;
		double mouseX, mouseY;
		int tick = 0;
		int desiredState = 0;

		std::vector<std::string> args;
		
		//State Transitions
		virtual bool enter(const std::vector<std::string>& args) = 0;
		virtual bool exit() = 0;

		//Main loop functions
		virtual void network(int ms) = 0;
		virtual void update(float dt) = 0;
		virtual void render() = 0;

		//Make sure to call child destructors
		virtual ~Scene() = default;
};

#endif