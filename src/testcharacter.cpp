#include "TestCharacter.h"

void TestCharacter::start()
{
	readHurtboxesFromFile(hurtboxes, hitboxes, pushboxes, "scripts/hitboxes/testBoxes.hbox");
	loadScript("scripts/ROSE/CMEF.rose");
	loadScript("scripts/ROSE/test.rose");

	// animID = 0;
}

void TestCharacter::update(int tick)
{

}

void TestCharacter::enterState(int newstate, int oldstate)
{
	// exitState(oldstate, newstate);
	// switch(newstate) {
	// case idle:
	// 	// PlayAnimation(idleAnim);
	// 	// animID = 0;
	// 	std::cout << "entering idle" << std::endl;
	// 	break;
	// case atk:
	// 	std::cout << "entering atk" << std::endl;
	// 	break;
	// }
	// state = newstate;
}

void TestCharacter::exitState(int oldstate, int newstate)
{
	// switch(oldstate) {
	// case idle:
	// 	std::cout << "exiting idle" << std::endl;
	// 	break;
	// case atk:
	// 	std::cout << "exiting atk" << std::endl;
	// 	break;
	// }
} 