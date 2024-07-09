#include "InputHandler.h"

InputHandler::InputHandler()
{}

InputHandler::~InputHandler()
{}

void InputHandler::init()
{}

void InputHandler::registerInput(const std::bitset<4> &input)
{
	if((std::bitset<(buffer * 4)>(input.to_ullong()) << 4 & dirHold) == 0)
	{
		dirPress = dirPress | std::bitset<(buffer * 4)>(input.to_ullong());
	}
	dirHold = dirHold | std::bitset<(buffer * 4)>(input.to_ullong());
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

	bool executed = false;

	std::bitset<(buffer * 4)> dirCheck = dirPress;

	if(c.holdList[c.commandNumber] < 0)
	{
		dirCheck = dirHold;
	}

	if(c.commandNumber == c.commandList.size())
	{
		if(c.commandTimer > 0)
		{
			executed = true;
		}
		else
		{
			c.commandTimer = 0;
			c.commandNumber = 0;
		}
	}
	else{
		if(std::bitset<4>(dirCheck.to_ullong()) == c.commandList[c.commandNumber])
		{
			c.commandTimer = c.tE + 1;
			c.commandNumber++;
			c.lastInputTick = currentTick;
		}

	}

	if(c.commandTimer > 0)
		--c.commandTimer;
	else{
		if(c.commandNumber > 0)
			--c.commandNumber;
	}

	return executed;
}

bool InputHandler::checkCommand(const std::bitset<4> &direction, bool hold)
{

	bool executed = false;
	
	std::bitset<(buffer * 4)> dirCheck = hold ? dirHold : dirPress;

	if(std::bitset<4>(dirCheck.to_ullong()) == direction)
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

	for(int i = 0; i <= buffer; i++)
	{
		if(std::bitset<7>((butCheck.to_ullong() >> (7 * i))) == button)
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
	
	dirHold = dirHold << 4;
	dirPress = dirPress << 4;

	butHold = butHold << 7;
	butPress = butPress << 7;
}