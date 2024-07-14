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

class state
{
	
};

class IntroState : public GameState
{
public:

	TestCharacter* testChar;
	TestCharacter* testChar2;

	//Static accessor
	static IntroState* get();

	int len;
	unsigned char *buffer;

    bool save_char()
    {
        if (!testChar) {
            std::cout << "testChar is null\n";
            return false;
        }
        len = sizeof(TestCharacter);
        buffer = new unsigned char[len];
        if (!buffer)
        {
            std::cout << "save failed: memory allocation error\n";
            return false;
        }
        memcpy(buffer, testChar, len);
        std::cout << "save complete\n";
        return true;
    }

    bool load_char()
    {
        if (!buffer || len == 0) {
            std::cout << "load failed: no data to load\n";
            return false;
        }
        memcpy(testChar, buffer, len);
        std::cout << "load complete\n";
        return true;
    }

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
	