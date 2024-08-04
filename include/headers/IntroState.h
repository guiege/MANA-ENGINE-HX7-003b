#ifndef INTRO_STATE
#define INTRO_STATE

#include "imgui.h"

#include "GameState.h"
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

class IntroState : public GameState
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
	std::string charSavedState = ""; // Save these to avoid string corruption with the shallow copy
	std::string charSavedSubroutines = "";
	std::string charSavedState2 = "";
    std::string charSavedSubroutines2 = "";

    bool save_char()
    {
        if (!testChar) {
            std::cout << "testChar is null\n";
            return false;
        }
        len = sizeof(TestCharacter);
        buffer = new unsigned char[len];
        charSavedState = testChar->currentState;
        charSavedSubroutines = testChar->subroutines;
        if (!buffer)
        {
            std::cout << "save failed: memory allocation error\n";
            return false;
        }
        memcpy(buffer, testChar, len);

        len2 = sizeof(TestCharacter);
        buffer2 = new unsigned char[len];
        charSavedState2 = testChar2->currentState;
        charSavedSubroutines2 = testChar2->subroutines;
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
        testChar->currentState = charSavedState;
        testChar->subroutines = charSavedSubroutines;

        if (!buffer2 || len2 == 0) {
            std::cout << "load failed: no data to load\n";
            return false;
        }
        memcpy(testChar2, buffer2, len2);
        testChar2->currentState = charSavedState2;
        testChar2->subroutines = charSavedSubroutines2;
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

	glm::mat4 proj;
	glm::mat4 worldProj;
	glm::mat4 uiProj;

	// int buttonCount;
	// const unsigned char *buttons;

	//Camera Shake Variables
	glm::vec2 maxOffset = {80.0f, 80.0f};
	float maxAngle = 5.0f; // 10 Degrees max shake angle
	float trauma = 0.0f; // shake is trauma^2 or trauma^3
	const siv::PerlinNoise::seed_type seed = 123456u;
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

	std::vector<Solid> solids;
	std::vector<Actor*> actors;

	std::vector<int> spdH{-1, 1, 1, 1};
	std::vector<std::bitset<4>> spdC{FK_Input_Buttons.FORWARD, FK_Input_Buttons.DOWN, FK_Input_Buttons.BACK, FK_Input_Buttons.UP};
};

#endif
	