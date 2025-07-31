#ifndef INTRO_STATE
#define INTRO_STATE

#include "imgui.h"

#include "Scene.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <deque>
#include <windows.h>

#include <chrono>
#include "PerlinNoise.h"

#include "Shader.h"
#include "Renderer.h"
#include "Texture.h"
#include "Sprite.h"
#include "Actor.h"
#include "Solid.h"
#include "PostProcessor.h"
#include "InputHandler.h"
#include "Camera.h"

#include "GameState.h"
#include "NonGameState.h"

class IntroState : public Scene
{
public:

	Spritesheet* icons;
	Spritesheet* gauge;

	int len;
	int checksum = 0;
	unsigned char *buffer;

	//Static accessor
	static IntroState* get();

	//Transitions
	bool enter();
	bool exit();

	int ReadInputs();
	int ReadControllerInputs();

	//Main loop functions
	void update(float dt);
	void render();

	//GGPO compliant functions
	void VectorWar_Init(unsigned short localport, int num_players, GGPOPlayer *players, int num_spectators);
	void VectorWar_InitSpectator(unsigned short localport, int num_players, char *host_ip, unsigned short host_port);
	void VectorWar_DrawCurrentFrame();
	void VectorWar_AdvanceFrame(int inputs[], int disconnect_flags);
	void VectorWar_RunFrame();
	void VectorWar_Idle(int time);
	void VectorWar_DisconnectPlayer(int player);
	void VectorWar_Exit();

private:
	//Static instance
	static IntroState sIntroState;

	//Private constructor
	IntroState();

	Renderer* batchRenderer;
	Renderer* characterRenderer;
	Renderer* shadowRenderer;

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

	const std::uint32_t seed = static_cast<std::uint32_t>(
        std::chrono::steady_clock::now().time_since_epoch().count()
    );
	// const siv::PerlinNoise::seed_type seed = 654321u;
	const siv::PerlinNoise perlin{ seed };

	//Input History Display Variables
	std::unordered_map<int, std::vector<int>> inputHistory;
	static const int MAX_HISTORY_SIZE = 22;
	int currentIndex = 0;
	glm::vec2 dirHistoryPos = {20, 125};
	glm::vec2 butHistoryPos = {52, 125};

	//CAMERA CONTROLS
	float baseScale = 0.8f;
    float maxScale = 2.0f;
    float scale = 0.8f;



	float cameraXPos = 0;
	float cameraYPos = 0;
	float cameraRot = 0.0f;
	float cameraScale = 1.0f;
	float cameraSnap = 0.1f;
	double lastX = 0.0, lastY = 0.0;
	bool isPanning = false;
	int cameraMinPos = 1000;
	int cameraMaxPos = 3800;
	float shake = 0.0f;
	float offsetX = 0.0f;
	float offsetY = 0.0f;
	float angle = 0.0f;

};

#define ARRAY_SIZE(n)      (sizeof(n) / sizeof(n[0]))
#define FRAME_DELAY        2

#endif