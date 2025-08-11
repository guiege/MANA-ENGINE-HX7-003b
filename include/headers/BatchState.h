#pragma once

#include "imgui.h"

#include "Scene.h"

#include "Camera.h"

class BatchState : public Scene
{
public:
	//Static accessor
	static BatchState* get();

	//Transitions
	bool enter(const std::vector<std::string>& args);
	bool exit();

	//Main loop functions
	void network(int ms);
	void update(float dt);
	void render();

private:
	//Static instance
	static BatchState sBatchState;

	//Private constructor
	BatchState();
};