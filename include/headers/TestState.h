#ifndef TEST_STATE
#define TEST_STATE

#include "imgui.h"

#include "GameState.h"
#include "Shader.h"
#include "Renderer.h"
#include "Texture.h"
#include "Sprite.h"
#include "Spritesheet.h"
#include "Actor.h"
#include "Solid.h"
#include "PostProcessor.h"
#include "InputHandler.h"
#include "Camera.h"
#include "Util.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;


class TestState : public GameState
{
public:
	//Static accessor
	static TestState* get();

	//Transitions
	bool enter();
	bool exit();

	//Main loop functions
	void update(float dt);
	void render();

private:
	//Static instance
	static TestState sTestState;

	//Private constructor
	TestState();

	Renderer* batchRenderer;
};

#endif
	