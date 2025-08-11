#pragma once

#include "imgui.h"
#include "fmod.h"
#include "fmod_common.h"
#include <vector>

#include "Scene.h"
#include "PostProcessor.h"

class OnlineLobby : public Scene
{
public:
	static OnlineLobby* get();

	bool enter(const std::vector<std::string>& args);
	bool exit();

	void network(int ms);
	void update(float dt);
	void render();

private:

	char remoteIP[64] = "";

	static OnlineLobby sOnlineLobby;

	FMOD_SYSTEM* system;
	FMOD_SOUND* sound;
	FMOD_CHANNEL*channel;

	Renderer* batchRenderer;

	OnlineLobby();
};