#pragma once

#include "Util.h"

#include <vector>
#include <iostream>
#include <bitset>
#include <algorithm>

const int buffer = 5;

class InputHandler {
public:

	int commandTimer = 0;

	int commandNumber = 0;

	int currentTick = 0;

	int lastInputTick = 0;

	std::string currentDirection = "NEUTRAL";
	std::string oldDirection = "NEUTRAL";
	int currentInputTimer = 0;

				    //tick2|   tick1|
	std::bitset<(buffer * 4)> dir; //UDBF
	std::bitset<(buffer * 7)> but; //xyzsabc(LP MP HP START LK MK HK)

	std::bitset<(buffer * 4)> dirHold;
	std::bitset<(buffer * 4)> dirPress;
	std::bitset<(buffer * 4)> dirRelease;

	std::bitset<(buffer * 7)> butHold;
	std::bitset<(buffer * 7)> butPress;
	std::bitset<(buffer * 7)> butRelease;

	InputHandler();
	~InputHandler();

	void init();

	void registerInputs(const int inputs);

	void registerInput(const std::bitset<4> &input);

	void registerInput(const std::bitset<7> &input);

	bool checkCommand(CommandSequence &c);

	bool checkCommand(const std::bitset<4> &direction, bool hold);

	bool checkCommandStrict(const std::bitset<4> &direction, bool hold);

	bool checkBuffer(const std::bitset<4> &direction, bool hold);

	bool checkCommand(const std::bitset<7> &button, bool hold);

	void update(const int &tick);

	bool inputChanged = false;

};

enum ButtonIDs {
    UP          = (1 << 3),
    DOWN        = (1 << 2),
    BACK        = (1 << 1),
    FORWARD     = (1 << 0),
    UP_FORWARD  = UP | FORWARD,
    UP_BACK     = UP | BACK,
    DOWN_FORWARD = DOWN | FORWARD,
    DOWN_BACK   = DOWN | BACK,
    LP          = (1 << 10),
    MP          = (1 << 9),
    HP          = (1 << 8),
    START       = (1 << 7),
    LK          = (1 << 6),
    MK          = (1 << 5),
    HK          = (1 << 4)
};

struct {

	std::bitset<4> UP 			= (1 << 3);
	std::bitset<4> DOWN 		= (1 << 2); 
	std::bitset<4> BACK 		= (1 << 1);
	std::bitset<4> FORWARD 		= (1 << 0);
	std::bitset<4> UP_FORWARD 	= (UP | FORWARD);
	std::bitset<4> UP_BACK 		= (UP | BACK);
	std::bitset<4> DOWN_FORWARD = (DOWN | FORWARD);
	std::bitset<4> DOWN_BACK	= (DOWN | BACK);

	std::bitset<7> LP 			= (1 << 6);
	std::bitset<7> MP 			= (1 << 5);
	std::bitset<7> HP 			= (1 << 4);
	std::bitset<7> START 		= (1 << 3);
	std::bitset<7> LK 			= (1 << 2);
	std::bitset<7> MK 			= (1 << 1);
	std::bitset<7> HK 			= (1 << 0);

} FK_Input_Buttons;