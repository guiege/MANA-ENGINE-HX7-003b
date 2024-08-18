#ifndef INTRO_STATE
#define INTRO_STATE

#include "imgui.h"

#include "Scene.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <deque>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "PerlinNoise.h"

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

class IntroState : public Scene
{
public:

	TestCharacter* testChar;
	TestCharacter* testChar2;

	Spritesheet* icons;
	Spritesheet* gauge;

	//Static accessor
	static IntroState* get();

	int len;
	int len2;
	unsigned char *buffer;
	unsigned char *buffer2;

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

        len2 = sizeof(TestCharacter);
        buffer2 = new unsigned char[len];
        if (!buffer2)
        {
            std::cout << "save failed: memory allocation error\n";
            return false;
        }
        memcpy(buffer2, testChar2, len2);
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

        if (!buffer2 || len2 == 0) {
            std::cout << "load failed: no data to load\n";
            return false;
        }
        memcpy(testChar2, buffer2, len2);
        std::cout << "load complete\n";
        return true;
    }

	//Transitions
	bool enter();
	bool exit();

	int ReadInputs();

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

	bool savingLoading = false;

	glm::mat4 proj;
	glm::mat4 worldProj;
	glm::mat4 uiProj;

	// int buttonCount;
	// const unsigned char *buttons;

	//Camera Shake Variables
	glm::vec2 maxOffset = {150.0f, 150.0f};
	float maxAngle = 5.0f; // 10 Degrees max shake angle
	float trauma = 0.0f; // shake is trauma^2 or trauma^3
	const siv::PerlinNoise::seed_type seed = 654321u;
	const siv::PerlinNoise perlin{ seed };

	//Input History Display Variables
	std::unordered_map<int, std::vector<int>> inputHistory;
	static const int MAX_HISTORY_SIZE = 22;
	int currentIndex = 0;
	glm::vec2 dirHistoryPos = {20, 125};
	glm::vec2 butHistoryPos = {52, 125};

	//CAMERA CONTROLS
	float cameraXPos = 0;
	float cameraYPos = 0;
	float cameraRot = 0.0f;
	float cameraScale = 1.0f;
	double lastX = 0.0, lastY = 0.0;
	bool isPanning = false;
	int cameraMinPos = 40;
	int cameraMaxPos = 3960;
	float shake = 0.0f;
	float offsetX = 0.0f;
	float offsetY = 0.0f;
	float angle = 0.0f;

	std::vector<Solid> solids;
	std::vector<Actor*> actors;

	std::vector<int> spdH{-1, 1, 1, 1};
	std::vector<std::bitset<4>> spdC{FK_Input_Buttons.FORWARD, FK_Input_Buttons.DOWN, FK_Input_Buttons.BACK, FK_Input_Buttons.UP};
};

#endif