#include "InputHandler.h"

InputHandler::InputHandler()
{}

InputHandler::~InputHandler()
{}

void InputHandler::init()
{}

void InputHandler::registerInputs(const int inputs) //(buttons)0000000 (dirs)0000
{
	std::bitset<4> directions(inputs);
	std::bitset<7> buttons(inputs >> 4);

	registerInput(directions);
	registerInput(buttons);
}

void InputHandler::registerInput(const std::bitset<4> &input)
{
	std::bitset<4> currentInput = input;
	if((input & FK_Input_Buttons.BACK).any() && (input & FK_Input_Buttons.FORWARD).any()){
		currentInput &= ~(FK_Input_Buttons.BACK | FK_Input_Buttons.FORWARD);
	}
	if((input & FK_Input_Buttons.UP).any() && (input & FK_Input_Buttons.DOWN).any()){
		currentInput &= ~(FK_Input_Buttons.UP | FK_Input_Buttons.DOWN);
	}

	std::bitset<4> oldHeld = std::bitset<4>((dirHold >> 4).to_ulong());
	std::bitset<4> newPresses = (currentInput & ~oldHeld);
	std::bitset<4> released = (oldHeld & ~currentInput);
	std::bitset<4> stillHeld = (currentInput & oldHeld);

	std::bitset<4> reactivePresses;
	if (released.any()) {
	    reactivePresses = stillHeld;
	}

	std::bitset<4> finalPresses = newPresses | reactivePresses;

	if (finalPresses.any()) {
		dirPress = dirPress | std::bitset<(buffer * 4)>(currentInput.to_ullong());
	}

	dirHold = dirHold | std::bitset<(buffer * 4)>(currentInput.to_ullong());
}

void InputHandler::registerInput(const std::bitset<7> &input)
{

	if((std::bitset<(buffer * 7)>(input.to_ullong()) << 7 & butHold) == 0)
	{
		butPress = butPress | std::bitset<(buffer * 7)>(input.to_ullong());
	}
	butHold = butHold | std::bitset<(buffer * 7)>(input.to_ullong());

}

bool InputHandler::checkCommand(CommandSequence &c)
{
	if(c.timingList[0] == 0){
		return checkCommandStrict(c.commandList[0], 1);
	}

	if(c.timingList[0] == 50){
		return checkCommand(c.commandList[0],1);
	}

	bool executed = false;
	std::bitset<(buffer * 4)> dirCheck = dirPress;
	if(sgn(c.timingList[c.commandNumber]) < 0)
	{
		dirCheck = dirHold;
	}

	if(c.commandNumber == c.commandList.size())
	{
		c.executeTimer = c.commandTimer;
		c.commandTimer = 0;
		c.commandNumber = 0;
	}

	if(c.executeTimer > 0){
		executed = true;
	}


	std::bitset<4> checkset((dirCheck).to_ullong());
	if(checkset == c.commandList[c.commandNumber]){
		c.commandTimer = abs(c.timingList[c.commandNumber]) + 1;
		c.commandNumber++;
	} else{
		if(checkset.any()){
			if(c.commandNumber > 0){
				if(c.commandList[c.commandNumber-1] == 0){
					std::cout << "aw dang it" << std::endl;
					c.commandTimer = 0;
					c.commandNumber = 0;
				}
			}
		}
	}

	if(c.executeTimer > 0)
		--c.executeTimer;

	if(c.commandTimer > 0)
		--c.commandTimer;
	else{
		if(c.commandNumber > 0)
			c.commandNumber = 0;
	}

	return executed;
}

bool InputHandler::checkCommandStrict(const std::bitset<4> &direction, bool hold)
{

	bool executed = false;
	
	std::bitset<(buffer * 4)> dirCheck = hold ? dirHold : dirPress;

	if(std::bitset<4>(dirCheck.to_ullong()) == direction)
		executed = true;

	return executed;
}

bool InputHandler::checkCommand(const std::bitset<4> &direction, bool hold)
{

	bool executed = false;
	
	std::bitset<(buffer * 4)> dirCheck = hold ? dirHold : dirPress;

	if ((dirCheck.to_ullong() & direction.to_ullong()))
		executed = true;

	return executed;
}

bool InputHandler::checkCommand(const std::bitset<7> &button, bool hold)
{

	bool executed = false;

	std::bitset<(buffer * 7)> butCheck = butPress;

	if(hold)
	{
		butCheck = butHold;
	}

	for(int i = 0; i < buffer; i++)
	{
		if ((butCheck.to_ullong() >> (7 * i)) & button.to_ullong())
			executed = true;
	}

	return executed;
}

void InputHandler::update(const int &tick)
{

	currentTick = tick;

	if(currentInputTimer < 99)
		currentInputTimer++;

	std::bitset<4> currentDirPress = (dirHold >> 4).to_ullong();

	std::string newInput;

	inputChanged = false;

	oldDirection = currentDirection;

	if (currentDirPress == FK_Input_Buttons.UP) {
	    newInput = "UP";
	} else if (currentDirPress == FK_Input_Buttons.BACK) {
	    newInput = "BACK";
	} else if (currentDirPress == FK_Input_Buttons.FORWARD) {
	    newInput = "FORWARD";
	} else if (currentDirPress == FK_Input_Buttons.DOWN) {
	    newInput = "DOWN";
	} else if (currentDirPress == FK_Input_Buttons.UP_FORWARD) {
	    newInput = "UP_FORWARD";
	} else if (currentDirPress == FK_Input_Buttons.UP_BACK) {
	    newInput = "UP_BACK";
	} else if (currentDirPress == FK_Input_Buttons.DOWN_FORWARD) {
	    newInput = "DOWN_FORWARD";
	} else if (currentDirPress == FK_Input_Buttons.DOWN_BACK) {
	    newInput = "DOWN_BACK";
	} else {
	    newInput = "NEUTRAL";
	}

	if(newInput != currentDirection)
		currentInputTimer = 0;
	currentDirection = newInput;

	if(oldDirection != currentDirection)
		inputChanged = true;

	// std::cout<< dirPress << std::endl;
	
	dirHold = dirHold << 4;
	dirPress = dirPress << 4;

	butHold = butHold << 7;
	butPress = butPress << 7;
}