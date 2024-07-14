#include "TestCharacter.h"


void TestCharacter::start()
{
	std::ifstream file("res/hitboxes/testAnims.json");

	animations = json::parse(file);

	idleAnim = getAnimFromJson(animations, 0);
	jabAnim = getAnimFromJson(animations, 1);

	PlayAnimation(idleAnim);
	animID = 0;
}

void TestCharacter::update(int tick)
{

	switch(state) {
	case idle:
		// std::cout << "in idle state" << std::endl;
		
		if(inputHandler->checkCommand(FK_Input_Buttons.BACK, true))
			MoveX(-20);
		if(inputHandler->checkCommand(FK_Input_Buttons.FORWARD, true))
			MoveX(20);
		if(inputHandler->checkCommand(FK_Input_Buttons.UP, true))
			MoveY(-100);

		if(inputHandler->checkCommand(FK_Input_Buttons.LP, false)){
			PlayAnimation(jabAnim);
			animID = 1;
			enterState(atk, state);
		}
		break;
	case atk:
		// std::cout << "in atk state" << std::endl;
		if(currentAnim.finished){
			enterState(idle, state);
		}
		break;
	}

	switch(animID){
		case 0:
			currentAnim = idleAnim;
			break;
		case 1:
			currentAnim = jabAnim;
			break;
	}

}

void TestCharacter::enterState(int newstate, int oldstate)
{
	exitState(oldstate, newstate);
	switch(newstate) {
	case idle:
		PlayAnimation(idleAnim);
		animID = 0;
		std::cout << "entering idle" << std::endl;
		break;
	case atk:
		std::cout << "entering atk" << std::endl;
		break;
	}
	state = newstate;
}

void TestCharacter::exitState(int oldstate, int newstate)
{
	switch(oldstate) {
	case idle:
		std::cout << "exiting idle" << std::endl;
		break;
	case atk:
		std::cout << "exiting atk" << std::endl;
		break;
	}
} 