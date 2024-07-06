#pragma once

#include "imgui.h"

#include "GameState.h"

#include "Camera.h"

class BatchState : public GameState
{
public:
	//Static accessor
	static BatchState* get();

	//Transitions
	bool enter();
	bool exit();

	//Main loop functions
	void update(float dt);
	void render();

private:
	//Static instance
	static BatchState sBatchState;

	//Private constructor
	BatchState();
};