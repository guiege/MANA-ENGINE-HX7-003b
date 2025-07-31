#include "GameState.h"

void GameState::init()
{
	i1 = new InputHandler();
	i2 = new InputHandler();

	_chars[0] = Character(i1, ResourceManager::GetTexture("hydesheet"), "res/textures/hydesheet.json", 1500, 0, 4296, 15673, 0);
	_chars[0].init();

	_chars[1] = Character(i2, ResourceManager::GetTexture("hydesheet"), "res/textures/hydesheet.json", 2500, 0, 4296, 15673, 0);
	_chars[1].init();

	// testChar = new Character(i1, ResourceManager::GetTexture("hydesheet"), "res/textures/hydesheet.json", 1500, 0, 4296, 15673, 0);
	// testChar->init();

	// testChar2 = new Character(i2, ResourceManager::GetTexture("hydesheet"), "res/textures/hydesheet.json", 2500, 0, 4296, 15673, 0);
	// testChar2->init();
}

void GameState::parsePlayerInputs(int inputs, int i, InputHandler* ih)
{
	
}

void GameState::update(int inputs[], int disconnect_flags)
{
	_framenumber++;

	i1->registerInputs(inputs[0]);
	i2->registerInputs(inputs[1]);

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

	i1->update(_framenumber);
	i2->update(_framenumber);


}