#ifndef INTRO_STATE
#define INTRO_STATE

#include "imgui.h"

#include "GameState.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Renderer.h"
#include "Texture.h"
#include "Sprite.h"
#include "Actor.h"
#include "Solid.h"
#include "PostProcessor.h"
#include "InputHandler.h"
#include "Character.h"
#include "Camera.h"

#include "TestCharacter.h"

class IntroState : public GameState
{
public:
	//Static accessor
	static IntroState* get();

	//Transitions
	bool enter();
	bool exit();

	//Main loop functions
	void update(float dt);
	void render();

private:
	//Static instance
	static IntroState sIntroState;

	//Private constructor
	IntroState();

	Renderer* batchRenderer;
	Renderer* characterRenderer;

	InputHandler* inputHandler;
	InputHandler* inputHandler2;

	Camera* m_Camera;

	//CAMERA CONTROLS
	float cameraXPos = 0;
	float cameraYPos = 0;
	float cameraRot = 0.0f;
	float cameraScale = 1.0f;
	double lastX = 0.0, lastY = 0.0;
	bool isPanning = false;
};

#endif
	