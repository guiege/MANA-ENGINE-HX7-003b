#ifndef _GAMESTATE_H_
#define _GAMESTATE_H_

#include "imgui.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <deque>
#include <chrono>

#include "Actor.h"
#include "InputHandler.h"
#include "Character.h"
#include "ResourceManager.h"

#define MAX_SHIPS               2

struct GameState{
	void init();

	void update(int inputs[], int disconnect_flags);

	int         _framenumber;
	float trauma = 0.0f; // shake is trauma^2 or trauma^3
	Character   _chars[MAX_SHIPS];
	InputHandler _handlers[MAX_SHIPS];

};


#endif