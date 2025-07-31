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
	void parsePlayerInputs(int inputs, int i, InputHandler* ih);

	int         _framenumber;
	Character   _chars[MAX_SHIPS];
	InputHandler* i1;
	InputHandler* i2;

};


#endif