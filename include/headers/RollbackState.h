#ifndef ROLLBACK_STATE
#define ROLLBACK_STATE

#include "ggponet.h"

#include "imgui.h"

#include "GameState.h"

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

	void update(float dt);
	void render();

private:
	//Static instance
	static RollbackState sRollbackState;

	//Private constructor
	RollbackState();
};

void Rollback_Init(unsigned short localport, int num_players, GGPOPlayer *players, int num_spectators);
void Rollback_DrawCurrentFrame();
void Rollback_AdvanceFrame(int inputs[], int disconnect_flags);
void Rollback_RunFrame();
void Rollback_Idle(int time);
void Rollback_DisconnectPlayer(int player);
void Rollback_Exit();

#define FRAME_DELAY		2;

#endif