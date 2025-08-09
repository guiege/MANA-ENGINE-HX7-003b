#include "GameState.h"

void GameState::init()
{
	_handlers[0] = InputHandler();
	_handlers[1] = InputHandler();

	_chars[0] = Character(&_handlers[0], ResourceManager::GetTexture("hydesheet"), "res/textures/hydesheet.json", 1500, 0, 4296, 15673, 0);
	_chars[0].init();

	_chars[1] = Character(&_handlers[1], ResourceManager::GetTexture("hydesheet"), "res/textures/hydesheet.json", 2500, 0, 4296, 15673, 0);
	_chars[1].init();

	// testChar = new Character(i1, ResourceManager::GetTexture("hydesheet"), "res/textures/hydesheet.json", 1500, 0, 4296, 15673, 0);
	// testChar->init();

	// testChar2 = new Character(i2, ResourceManager::GetTexture("hydesheet"), "res/textures/hydesheet.json", 2500, 0, 4296, 15673, 0);
	// testChar2->init();
}

void GameState::update(int inputs[], int disconnect_flags)
{
	_framenumber++;

	_handlers[0].registerInputs(inputs[0]);
	_handlers[1].registerInputs(inputs[1]);

	std::string char1state = _chars[0].GetCurrentState();
	std::string char2state = _chars[1].GetCurrentState();

	_chars[1].updateScript(_framenumber, &_chars[0]);
	_chars[0].updateScript(_framenumber, &_chars[1]);

	if(_chars[0].checkCollision(&_chars[1])){
		_chars[0].hitOpponent(&_chars[1], char1state.c_str());
	} 

	// else if(testChar->checkClash(testChar2)){
	// 	std::cout << "clash happened" << std::endl;
	// }

	if(_chars[1].checkCollision(&_chars[0]))
	{
		_chars[1].hitOpponent(&_chars[0], char2state.c_str());
	}

	_chars[0].SetPushFlag(false);
	_chars[1].SetPushFlag(false);
	if(_chars[0].pos.x >= 4000){
		_chars[0].pos.x = 4000;
		_chars[0].SetPushFlag(true);
	}
	if(_chars[0].pos.x <= 0){
		_chars[0].pos.x = 0;
		_chars[0].SetPushFlag(true);
	}
	if(_chars[1].pos.x >= 4000){
		_chars[1].pos.x = 4000;
		_chars[1].SetPushFlag(true);
	}
	if(_chars[1].pos.x <= 0){
		_chars[1].pos.x = 0;
		_chars[1].SetPushFlag(true);
	}

	_handlers[0].update(_framenumber);
	_handlers[1].update(_framenumber);


}