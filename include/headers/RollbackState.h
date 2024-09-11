#ifndef ROLLBACK_STATE
#define ROLLBACK_STATE

#include <windows.h>
#include "ggponet.h"

#include "imgui.h"

#include "GameState.h"
#include "NonGameState.h"

#include "Scene.h"
#include "Shader.h"
#include "Renderer.h"
#include "TestCharacter.h"

class RollbackState : public Scene
{
public:

	TestCharacter* testChar;
	TestCharacter* testChar2;

	Spritesheet* icons;
	Spritesheet* gauge;

	//Static accessor
	static RollbackState* get();

	//Transitions
	bool enter();
	bool exit();

	//Main loop functions

	int ReadInputs();
	void update(float dt);
	void render();

	void Rollback_Init(unsigned short localport, int num_players, GGPOPlayer *players, int num_spectators);
	void Rollback_InitSpectator(unsigned short localport, int num_players, char *host_ip, unsigned short host_port);
	void Rollback_DrawCurrentFrame();
	void Rollback_RunFrame();
	void Rollback_Idle(int time);
	void Rollback_DisconnectPlayer(int player);
	void Rollback_Exit();

private:
	//Static instance
	static RollbackState sRollbackState;

	//Private constructor
	RollbackState();
};

void Rollback_AdvanceFrame(int inputs[], int disconnect_flags);

#define ARRAY_SIZE(n)      (sizeof(n) / sizeof(n[0]))
#define FRAME_DELAY		2

#endif