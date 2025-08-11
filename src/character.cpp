#include "Character.h"

//“When you cross up someone, instead of having them face toward your character, face them the opposite direction from the way that the attacker is facing”
//if(crossup)
// opponent->SetFlipped(!flipped);

//Trades
//When two players hit each other with the same move. Allow players to stay in attack/active frames, not hit stun frames. This makes it easier to tell what hit they did. Otherwise no commentator would tell which move was used
//I think I already do this

//PREVENT UNBLOCKABLES!!!
//For a move that is explicitly unblockable, make sure that if the opponent is already in blockstun they are blockable.

//For crossups, you should ALWAYS have to block away from the opponent point character no matter what other crazy stuff is going on on the screen. This is to avoid having to block from two directions at once.

//For high low, light blue hurtboxes = blocking high, dark blue hurtboxes = blocking low. 
//During the hitstop from blocking a hit, you get pink hurtboxes meaning that you are blocking both ways, high and low. ONLY DURING THE HITSTOP, it goes back to normal afterwards so you have to guess

//ONLY ALLOW THE CHARACTER TO BE HIT BY ONE THING PER FRAME. This is to prevent strike throw unblockables.

//TODO change the character from rectangle collision to a point in space, keep the rectangle as a pushbox.
//Have a 'point' that is at the feet of the character. This will determine collision with the ground. During certain(mostly aerial) moves, make it so that the point can go through the floor so you can hit a short character

//Try playing a hit grunt sound(one of a set) after every hit, it is the way that old games worked and apparently it's better. Test both randomly and every hit.

//No preblock before hits. Adding a frame of preblock feels awful. Make it so that if you are in a state that can block and you are holding backwards, you are able to block.

//Stop the clock whenever characters are doing a cinematic
//Make it so that you can't die in the middle of cinematics, it ruins the presentation

//The person throwing another character should not be able to mess with the other character while in the throw(think mvc2 shenanigans)

//How to handle hitting multiple things: if the attack connects with 2 objects at the same time, it hits both. If the attack connects and then a few frames pass and then it connects with another object, it is used up.

//Crouch tech: decide whether it should exist(leaning towards no)

//SOCD cleaning, add it.

//DP fireball priority problem

//There is no reason ever to have a player to do a half circle, just use qc in the direction you want. Think donkey kick from SF6. Acceptable move types: qc, dp, 360, and charge up/back.

//Guard breaks should never open you up if you are blocking correctly. R. I. S. C. from gg is a good solution as it makes the scaling on the next combo greater depending on how much you have blocked. You can block
//forever if you guess correctly.

//Stun: getting hit more for getting hit alot is just not fun. Notable examples of good design: Marvel 2 let you get out of combos if you built enough stun.

//TODO: get the fireball that moves on the beat working in game and see what consequences it has. More ideas will come naturally from there. Get overheads working and make it so that overheads only come out when executed off beat. They don't have to
		//be telegraphed as much because you breaking the rhythm is an indicator.

void Character::init() //TODO: add commands addPositionX and addPositionY, addNamerakaMoveX and Y, velocityXPercentEachFrame, haltMomentum
{
	readHurtboxesFromFile(hurtboxes, hitboxes, pushboxes, "scripts/hitboxes/testBoxes.hbox");
	loadScript("scripts/ROSE/CMEF.rose");
	loadScript("scripts/ROSE/test.rose");

	_motions1[0] = MotionInput<1>("INPUT_236214");
	_motions1[0].validCommands[0] = CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.DOWN_FORWARD, FK_Input_Buttons.FORWARD, FK_Input_Buttons.DOWN, FK_Input_Buttons.DOWN_BACK, FK_Input_Buttons.BACK},{-5,10,10,10,10,10});

	_motions1[1] = MotionInput<1>("INPUT_63214");
	_motions1[1].validCommands[0] = CommandSequence({FK_Input_Buttons.FORWARD, FK_Input_Buttons.DOWN_FORWARD, FK_Input_Buttons.DOWN_BACK, FK_Input_Buttons.BACK},{-5,12,12,12});

	_motions1[2] = MotionInput<1>("INPUT_421");
	_motions1[2].validCommands[0] = CommandSequence({FK_Input_Buttons.BACK, FK_Input_Buttons.DOWN, FK_Input_Buttons.DOWN_BACK},{-8,12,7});

	_motions1[3] = MotionInput<1>("INPUT_623");
	_motions1[3].validCommands[0] = CommandSequence({FK_Input_Buttons.FORWARD, FK_Input_Buttons.DOWN, FK_Input_Buttons.DOWN_FORWARD},{-8,-12,-7});
	
	_motions1[4] = MotionInput<1>("INPUT_236");
	_motions1[4].validCommands[0] = CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.DOWN_FORWARD, FK_Input_Buttons.FORWARD},{-5,10,10});

	_motions1[5] = MotionInput<1>("INPUT_214");
	_motions1[5].validCommands[0] = CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.DOWN_BACK, FK_Input_Buttons.BACK},{-5,10,10});

	_motions1[6] = MotionInput<1>("INPUT_CHARGE_BACK_FORWARD_30F");
	_motions1[6].validCommands[0] = CommandSequence({FK_Input_Buttons.BACK, FK_Input_Buttons.FORWARD},{130,10});

	_motions1[7] = MotionInput<1>("INPUT_44");
	_motions1[7].validCommands[0] = CommandSequence({FK_Input_Buttons.BACK, FK_Input_Buttons.BACK},{10,10});

	_motions1[8] = MotionInput<1>("INPUT_2");
	_motions1[8].validCommands[0] = CommandSequence({FK_Input_Buttons.DOWN},{0});

	_motions1[9] = MotionInput<1>("INPUT_3");
	_motions1[9].validCommands[0] = CommandSequence({FK_Input_Buttons.DOWN_FORWARD},{0});

	_motions1[10] = MotionInput<1>("INPUT_4");
	_motions1[10].validCommands[0] = CommandSequence({FK_Input_Buttons.BACK},{0});

	_motions1[11] = MotionInput<1>("INPUT_6");
	_motions1[11].validCommands[0] = CommandSequence({FK_Input_Buttons.FORWARD},{0});

	_motions1[12] = MotionInput<1>("INPUT_287");
	_motions1[12].validCommands[0] = CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.UP_BACK},{-12,10});

	_motions1[13] = MotionInput<1>("INPUT_289");
	_motions1[13].validCommands[0] = CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.UP_FORWARD},{-12,10});

	_motions1[14] = MotionInput<1>("INPUT_28");
	_motions1[14].validCommands[0] = CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.UP},{-12,10});

	_motions1[15] = MotionInput<1>("INPUT_ANY_UP");
	_motions1[15].validCommands[0] = CommandSequence({FK_Input_Buttons.UP},{50});

	_motions1[16] = MotionInput<1>("INPUT_ANY_DOWN");
	_motions1[16].validCommands[0] = CommandSequence({FK_Input_Buttons.DOWN},{50});

	_motions1[17] = MotionInput<1>("INPUT_ANY_BACK");
	_motions1[17].validCommands[0] = CommandSequence({FK_Input_Buttons.BACK},{50});

	_motions1[18] = MotionInput<1>("INPUT_ANY_FORWARD");
	_motions1[18].validCommands[0] = CommandSequence({FK_Input_Buttons.FORWARD},{50});

	_motions1[19] = MotionInput<1>("INPUT_8");
	_motions1[19].validCommands[0] = CommandSequence({FK_Input_Buttons.UP},{0});

	_motions1[20] = MotionInput<1>("INPUT_7");
	_motions1[20].validCommands[0] = CommandSequence({FK_Input_Buttons.UP_BACK},{0});

	_motions1[21] = MotionInput<1>("INPUT_9");
	_motions1[21].validCommands[0] = CommandSequence({FK_Input_Buttons.UP_FORWARD},{0});

	_motions1[22] = MotionInput<1>("INPUT_HOLD_2");
	_motions1[22].validCommands[0] = CommandSequence({FK_Input_Buttons.DOWN},{0});

	_motions1[23] = MotionInput<1>("INPUT_HOLD_1");
	_motions1[23].validCommands[0] = CommandSequence({FK_Input_Buttons.DOWN_BACK},{0});

	_motions1[24] = MotionInput<1>("INPUT_HOLD_3");
	_motions1[24].validCommands[0] = CommandSequence({FK_Input_Buttons.DOWN_FORWARD},{0});

	_motions1[25] = MotionInput<1>("INPUT_HOLD_4");
	_motions1[25].validCommands[0] = CommandSequence({FK_Input_Buttons.BACK},{0});

	_motions1[26] = MotionInput<1>("INPUT_HOLD_6");
	_motions1[26].validCommands[0] = CommandSequence({FK_Input_Buttons.FORWARD},{0});


	_motions3[0] = MotionInput<3>("INPUT_66");
	_motions3[0].validCommands[0] = CommandSequence({FK_Input_Buttons.FORWARD, 0, FK_Input_Buttons.FORWARD},{8,-8,1});
	_motions3[0].validCommands[1] = CommandSequence({FK_Input_Buttons.DOWN_FORWARD, 0, FK_Input_Buttons.FORWARD},{8,-8,1});
	_motions3[0].validCommands[2] = CommandSequence({FK_Input_Buttons.UP_FORWARD, 0, FK_Input_Buttons.FORWARD},{8,-8,1});

	// _motions3[1] = MotionInput<3>("INPUT_236");
	// _motions3[1].validCommands[0] = CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.FORWARD, FK_Input_Buttons.DOWN, FK_Input_Buttons.FORWARD},{-5,15,15,12});
	// _motions3[1].validCommands[1] = CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.BACK, FK_Input_Buttons.DOWN, FK_Input_Buttons.BACK},{-5,15,15,12});
	// _motions3[1].validCommands[2] = CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.BACK, FK_Input_Buttons.DOWN, FK_Input_Buttons.BACK},{-5,15,15,12});

	// _motions6[0] = MotionInput<6>("INPUT_214214");
	// _motions6[0].validCommands[0] =		CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.BACK, FK_Input_Buttons.DOWN, FK_Input_Buttons.BACK},{-5,15,15,12});
	// _motions6[0].validCommands[1] =		CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.BACK, FK_Input_Buttons.DOWN, FK_Input_Buttons.BACK},{-5,17,17,12});
	// _motions6[0].validCommands[2] =		CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.BACK, (FK_Input_Buttons.DOWN_FORWARD), FK_Input_Buttons.BACK},{-5,17,17,12});
	// _motions6[0].validCommands[3] =		CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.BACK, (FK_Input_Buttons.DOWN_BACK), FK_Input_Buttons.BACK},{-5,17,17,12});
	// _motions6[0].validCommands[4] =		CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.BACK, FK_Input_Buttons.DOWN, (FK_Input_Buttons.DOWN_BACK)},{-5,17,17,12});
	// _motions6[0].validCommands[5] =		CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.BACK, FK_Input_Buttons.DOWN, (FK_Input_Buttons.UP_BACK)},{-5,17,17,12});

	// _motions6[1] = MotionInput<6>("INPUT_236236");
	// _motions6[1].validCommands[0] =		CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.FORWARD, FK_Input_Buttons.DOWN, FK_Input_Buttons.FORWARD},{-5,15,15,12});
	// _motions6[1].validCommands[1] =		CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.FORWARD, FK_Input_Buttons.DOWN, FK_Input_Buttons.FORWARD},{-5,17,17,12});
	// _motions6[1].validCommands[2] =		CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.FORWARD, (FK_Input_Buttons.DOWN_FORWARD), FK_Input_Buttons.FORWARD},{-5,17,17,12});
	// _motions6[1].validCommands[3] =		CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.FORWARD, (FK_Input_Buttons.DOWN_BACK), FK_Input_Buttons.FORWARD},{-5,17,17,12});
	// _motions6[1].validCommands[4] =		CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.FORWARD, FK_Input_Buttons.DOWN, (FK_Input_Buttons.DOWN_FORWARD)},{-5,17,17,12});
	// _motions6[1].validCommands[5] =		CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.FORWARD, FK_Input_Buttons.DOWN, (FK_Input_Buttons.UP_FORWARD)},{-5,17,17,12});

	// _motions6[2] = MotionInput<6>("INPUT_623");
	// _motions6[2].validCommands[0] =		CommandSequence({FK_Input_Buttons.DOWN, 0,FK_Input_Buttons.DOWN},{-10,-10,10});
	// _motions6[2].validCommands[1] =		CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.UP,FK_Input_Buttons.DOWN},{-10,-10,10});
	// _motions6[2].validCommands[2] =		CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.BACK,FK_Input_Buttons.DOWN},{-10,-10,10});
	// _motions6[2].validCommands[3] =		CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.FORWARD,FK_Input_Buttons.DOWN},{-10,-10,10});
	// _motions6[2].validCommands[4] =		CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.UP_FORWARD,FK_Input_Buttons.DOWN},{-10,-10,10});
	// _motions6[2].validCommands[5] =		CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.UP_BACK,FK_Input_Buttons.DOWN},{-10,-10,10});

	insertMotionInput("INPUT_1080", {
		CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.BACK, FK_Input_Buttons.UP, FK_Input_Buttons.FORWARD}, {-70,70,70,70}),
		CommandSequence({FK_Input_Buttons.BACK, FK_Input_Buttons.UP, FK_Input_Buttons.FORWARD, FK_Input_Buttons.DOWN}, {-70,70,70,70}),
		CommandSequence({FK_Input_Buttons.UP, FK_Input_Buttons.FORWARD, FK_Input_Buttons.DOWN, FK_Input_Buttons.BACK}, {-70,70,70,70}),
		CommandSequence({FK_Input_Buttons.FORWARD, FK_Input_Buttons.DOWN, FK_Input_Buttons.BACK, FK_Input_Buttons.UP}, {-70,70,70,70}),

		CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.FORWARD, FK_Input_Buttons.UP, FK_Input_Buttons.BACK}, {-70,70,70,70}),
		CommandSequence({FK_Input_Buttons.FORWARD, FK_Input_Buttons.UP, FK_Input_Buttons.BACK, FK_Input_Buttons.DOWN}, {-70,70,70,70}),
		CommandSequence({FK_Input_Buttons.UP, FK_Input_Buttons.BACK, FK_Input_Buttons.DOWN, FK_Input_Buttons.FORWARD}, {-70,70,70,70}),
		CommandSequence({FK_Input_Buttons.BACK, FK_Input_Buttons.DOWN, FK_Input_Buttons.FORWARD, FK_Input_Buttons.UP}, {-70,70,70,70})
	});
	insertMotionInput("INPUT_720", {
		CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.BACK, FK_Input_Buttons.UP, FK_Input_Buttons.FORWARD}, {-45,45,45,45}),
		CommandSequence({FK_Input_Buttons.BACK, FK_Input_Buttons.UP, FK_Input_Buttons.FORWARD, FK_Input_Buttons.DOWN}, {-45,45,45,45}),
		CommandSequence({FK_Input_Buttons.UP, FK_Input_Buttons.FORWARD, FK_Input_Buttons.DOWN, FK_Input_Buttons.BACK}, {-45,45,45,45}),
		CommandSequence({FK_Input_Buttons.FORWARD, FK_Input_Buttons.DOWN, FK_Input_Buttons.BACK, FK_Input_Buttons.UP}, {-45,45,45,45}),

		CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.FORWARD, FK_Input_Buttons.UP, FK_Input_Buttons.BACK}, {-45,45,45,45}),
		CommandSequence({FK_Input_Buttons.FORWARD, FK_Input_Buttons.UP, FK_Input_Buttons.BACK, FK_Input_Buttons.DOWN}, {-45,45,45,45}),
		CommandSequence({FK_Input_Buttons.UP, FK_Input_Buttons.BACK, FK_Input_Buttons.DOWN, FK_Input_Buttons.FORWARD}, {-45,45,45,45}),
		CommandSequence({FK_Input_Buttons.BACK, FK_Input_Buttons.DOWN, FK_Input_Buttons.FORWARD, FK_Input_Buttons.UP}, {-45,45,45,45})
	});

	insertMotionInput("INPUT_360", {
		CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.BACK, FK_Input_Buttons.UP, FK_Input_Buttons.FORWARD}, {-30,30,30,30}),
		CommandSequence({FK_Input_Buttons.BACK, FK_Input_Buttons.UP, FK_Input_Buttons.FORWARD, FK_Input_Buttons.DOWN}, {-30,30,30,30}),
		CommandSequence({FK_Input_Buttons.UP, FK_Input_Buttons.FORWARD, FK_Input_Buttons.DOWN, FK_Input_Buttons.BACK}, {-30,30,30,30}),
		CommandSequence({FK_Input_Buttons.FORWARD, FK_Input_Buttons.DOWN, FK_Input_Buttons.BACK, FK_Input_Buttons.UP}, {-30,30,30,30}),

		CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.FORWARD, FK_Input_Buttons.UP, FK_Input_Buttons.BACK}, {-30,30,30,30}),
		CommandSequence({FK_Input_Buttons.FORWARD, FK_Input_Buttons.UP, FK_Input_Buttons.BACK, FK_Input_Buttons.DOWN}, {-30,30,30,30}),
		CommandSequence({FK_Input_Buttons.UP, FK_Input_Buttons.BACK, FK_Input_Buttons.DOWN, FK_Input_Buttons.FORWARD}, {-30,30,30,30}),
		CommandSequence({FK_Input_Buttons.BACK, FK_Input_Buttons.DOWN, FK_Input_Buttons.FORWARD, FK_Input_Buttons.UP}, {-30,30,30,30})
	});


	insertMotionInput("INPUT_632146",{
		CommandSequence({FK_Input_Buttons.FORWARD, FK_Input_Buttons.DOWN_FORWARD, FK_Input_Buttons.DOWN, FK_Input_Buttons.DOWN_BACK, FK_Input_Buttons.BACK, FK_Input_Buttons.FORWARD}, {-6,12,12,12,12,12}),
		CommandSequence({FK_Input_Buttons.FORWARD, FK_Input_Buttons.DOWN, FK_Input_Buttons.DOWN_BACK, FK_Input_Buttons.BACK, FK_Input_Buttons.FORWARD}, {-4,8,8,8,8}),
		CommandSequence({FK_Input_Buttons.FORWARD, FK_Input_Buttons.DOWN_FORWARD, FK_Input_Buttons.DOWN, FK_Input_Buttons.BACK, FK_Input_Buttons.FORWARD}, {-4,8,8,8,8}),
		CommandSequence({FK_Input_Buttons.FORWARD, FK_Input_Buttons.DOWN, FK_Input_Buttons.BACK, FK_Input_Buttons.FORWARD}, {-6,12,12,12}),
		CommandSequence({FK_Input_Buttons.FORWARD, FK_Input_Buttons.DOWN_FORWARD, FK_Input_Buttons.DOWN_BACK, FK_Input_Buttons.BACK, FK_Input_Buttons.FORWARD}, {-4,12,8,8,8})
	});

	insertMotionInput("INPUT_236214",{
		CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.DOWN_FORWARD, FK_Input_Buttons.FORWARD, FK_Input_Buttons.DOWN, FK_Input_Buttons.DOWN_BACK, FK_Input_Buttons.BACK},{-5,10,10,10,10,10})
	});	
	//all half circles(behemoths)
	insertMotionInput("INPUT_41236",{
		CommandSequence({FK_Input_Buttons.BACK, FK_Input_Buttons.DOWN_BACK, FK_Input_Buttons.DOWN, FK_Input_Buttons.DOWN_FORWARD, FK_Input_Buttons.FORWARD},{-5,12,12,12,12}),
		CommandSequence({FK_Input_Buttons.BACK, FK_Input_Buttons.DOWN, FK_Input_Buttons.DOWN_FORWARD, FK_Input_Buttons.FORWARD},{-5,12,12,12}),
		CommandSequence({FK_Input_Buttons.BACK, FK_Input_Buttons.DOWN_BACK, FK_Input_Buttons.DOWN, FK_Input_Buttons.FORWARD},{-5,12,12,12}),
		CommandSequence({FK_Input_Buttons.BACK, FK_Input_Buttons.DOWN_BACK, FK_Input_Buttons.DOWN_FORWARD, FK_Input_Buttons.FORWARD},{-5,12,12,12})
	});
	insertMotionInput("INPUT_63214",{
		CommandSequence({FK_Input_Buttons.FORWARD, FK_Input_Buttons.DOWN_FORWARD, FK_Input_Buttons.DOWN_BACK, FK_Input_Buttons.BACK},{-5,12,12,12})
	});
	insertMotionInput("INPUT_421",{
		CommandSequence({FK_Input_Buttons.BACK, FK_Input_Buttons.DOWN, FK_Input_Buttons.DOWN_BACK},{-8,12,7})
	});
	insertMotionInput("INPUT_623",{
		CommandSequence({FK_Input_Buttons.FORWARD, FK_Input_Buttons.DOWN, FK_Input_Buttons.DOWN_FORWARD},{-8,-12,-7})
	});
	insertMotionInput("INPUT_236",{
		CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.DOWN_FORWARD, FK_Input_Buttons.FORWARD},{-5,10,10})
	});
	insertMotionInput("INPUT_214",{
		CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.DOWN_BACK, FK_Input_Buttons.BACK},{-5,10,10})
	});
	insertMotionInput("INPUT_CHARGE_BACK_FORWARD_30F",{
		CommandSequence({FK_Input_Buttons.BACK, FK_Input_Buttons.FORWARD},{130,10})
	});

	insertMotionInput("INPUT_44",{
		CommandSequence({FK_Input_Buttons.BACK, FK_Input_Buttons.BACK},{10,10})
	});
	insertMotionInput("INPUT_66",{
		CommandSequence({FK_Input_Buttons.FORWARD, 0, FK_Input_Buttons.FORWARD},{8,-8,1}),
		CommandSequence({FK_Input_Buttons.DOWN_FORWARD, 0, FK_Input_Buttons.FORWARD},{8,-8,1}),
		CommandSequence({FK_Input_Buttons.UP_FORWARD, 0, FK_Input_Buttons.FORWARD},{8,-8,1})
	});
	insertMotionInput("INPUT_2",{
		CommandSequence({FK_Input_Buttons.DOWN},{0})
	});
	insertMotionInput("INPUT_3",{
		CommandSequence({FK_Input_Buttons.DOWN_FORWARD},{0})
	});
	insertMotionInput("INPUT_4",{
		CommandSequence({FK_Input_Buttons.BACK},{0})
	});
	insertMotionInput("INPUT_6",{
		CommandSequence({FK_Input_Buttons.FORWARD},{0})
	});
	insertMotionInput("INPUT_287",{
		CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.UP_BACK},{-12,10})
	});
	insertMotionInput("INPUT_289",{
		CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.UP_FORWARD},{-12,10})
	});
	insertMotionInput("INPUT_28",{
		CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.UP},{-12,10})
	});
	insertMotionInput("INPUT_ANY_UP",{
		CommandSequence({FK_Input_Buttons.UP},{50})
	});
	insertMotionInput("INPUT_ANY_DOWN",{
		CommandSequence({FK_Input_Buttons.DOWN},{50})
	});
	insertMotionInput("INPUT_ANY_BACK",{
		CommandSequence({FK_Input_Buttons.BACK},{50})
	});
	insertMotionInput("INPUT_ANY_FORWARD",{
		CommandSequence({FK_Input_Buttons.FORWARD},{50})
	});
	insertMotionInput("INPUT_8",{
		CommandSequence({FK_Input_Buttons.UP},{0})
	});
	insertMotionInput("INPUT_7",{
		CommandSequence({FK_Input_Buttons.UP_BACK},{0})
	});
	insertMotionInput("INPUT_9",{
		CommandSequence({FK_Input_Buttons.UP_FORWARD},{0})
	});

	insertMotionInput("INPUT_HOLD_2",{
		CommandSequence({FK_Input_Buttons.DOWN},{50})
	});
	insertMotionInput("INPUT_HOLD_1",{
		CommandSequence({FK_Input_Buttons.DOWN_BACK},{0})
	});
	insertMotionInput("INPUT_HOLD_3",{
		CommandSequence({FK_Input_Buttons.DOWN_FORWARD},{0})
	});
	insertMotionInput("INPUT_HOLD_4",{
		CommandSequence({FK_Input_Buttons.BACK},{0})
	});
	insertMotionInput("INPUT_HOLD_6",{
		CommandSequence({FK_Input_Buttons.FORWARD},{0})
	});

	buttons["INPUT_PRESS_LP"] = Button(FK_Input_Buttons.LP, false);
	buttons["INPUT_PRESS_MP"] = Button(FK_Input_Buttons.MP, false);
	buttons["INPUT_PRESS_HP"] = Button(FK_Input_Buttons.HP, false);

	buttons["INPUT_HOLD_LP"] = Button(FK_Input_Buttons.LP, true);
	buttons["INPUT_HOLD_MP"] = Button(FK_Input_Buttons.MP, true);
	buttons["INPUT_HOLD_HP"] = Button(FK_Input_Buttons.HP, true);

	buttons["INPUT_PRESS_LK"] = Button(FK_Input_Buttons.LK, false);
	buttons["INPUT_PRESS_MK"] = Button(FK_Input_Buttons.MK, false);
	buttons["INPUT_PRESS_HK"] = Button(FK_Input_Buttons.HK, false);

	buttons["INPUT_HOLD_LK"] = Button(FK_Input_Buttons.LK, true);
	buttons["INPUT_HOLD_MK"] = Button(FK_Input_Buttons.MK, true);
	buttons["INPUT_HOLD_HK"] = Button(FK_Input_Buttons.HK, true);

	initCommandMap();
	commandMap["exitState"] = [this](const std::vector<std::string>& params) {
    	exitState();
    };
    commandMap["landingStiffTime"] = [this](const std::vector<std::string>& params) {
    	landingStiffTime = stoi(params[0]);
    };

	// Add entries for buttons
	for (const auto& [key, value] : buttons) {
	    buttonMap[key] = false;
	}

    SetFlipped(false);

	int count = 0;

    for (const auto& [key, value] : states) {
	    stateIDs[key] = count;
	    stateNames[count] = key;
	    count++;
	}

	SetState("RoundInit");

	// afterImages.push_front(AfterImage(pos, spritesheet.GetFrame()));
	SetPushbox();
	centerVec = posOffset + glm::vec2(width/2, height);
}

void Character::SetState(const std::string& state)
{
	// std::cout << "setting state to: " << state << std::endl;
	bbscriptFrameCount = 0;
	framesUntilNextCommand = 0;
	currentLine = 0;
	currentState = stateIDs[state];
	firstFrame = true;
	states[GetCurrentState()].gatlingOptions.clear();
	states[GetCurrentState()].whiffCancelOptions.clear();
	hit = false;
	cancellable = false;
	recoveryState = false;
	stateTouchedGround = false;
	// stateLeftGround = false;
	velocityXPercentEachFrame = 1.0f;
	velocityYPercentEachFrame = 1.0f;
	carriedMomentumPercentage = 1.0f;
	velocityXRate = 1.0f;
	velocityYRate = 1.0f;
	gravity = initGravity;

	if(state == "CmnActStand"){
		characterState = 0;
		actionable = true;
		recoveryState = true;
	} else if(state == "CmnActFWalk"){
		states[GetCurrentState()].whiffCancelOptions.push_back("CmnActFDash");
	} else if(state == "CmnActCrouch2Stand"){
		characterState = 0;
	} else if(state == "CmnActJump"){
		characterState = 2;
	} else if(state == "CmnActJumpLanding"){
		characterState = 0;
		velocity.x = 0;
		velocity.y = 0;
	} else if(state == "CmnActLandingStiff"){
		characterState = 0;
		velocity.x = 0;
		velocity.y = 0;
	} else if(state == "CmnActFDash"){
		velocity.x = initDashFSpeed;
	} else if(state == "CmnActAirDash"){
		if(currentAirDashCount > 0)
			currentAirDashCount--;
	}

	jumpDir = 0;
	handleEvent(GetCurrentState(), "IMMEDIATE");
}

void Character::exitState()
{
	if(GetCurrentState() == "CmnActStand2Crouch"){
	    SetState("CmnActCrouch");
	} else if(GetCurrentState() == "CmnActJumpPre"){
		gravity = initGravity;
		velocity.y = -jumpHeight;
		if(jumpDir == 1)
			velocity.x = forwardJumpDistance;
		if(jumpDir == 2)
			velocity.x = -backwardJumpDistance;
		SetState("CmnActJump");
	} else if(GetCurrentState() == "CmnActAirDash"){
		SetState("CmnActJump");
	} else {
		if(characterState == 2)
			SetState("CmnActJump");
		else
			SetState("CmnActStand");
	}

}

void Character::executeCommands()
{
	if (framesUntilNextCommand == 0 && currentLine >= states[GetCurrentState()].instructions.size()) {
		handleEvent(GetCurrentState(), "BEFORE_EXIT");
		exitState();
	}
	
    if (framesUntilNextCommand > 0) {
        framesUntilNextCommand--;
    }

	while (framesUntilNextCommand == 0 && currentLine < states[GetCurrentState()].instructions.size()) {
	    executeInstruction(states[GetCurrentState()].instructions[currentLine]);
	    currentLine++;

	    if (framesUntilNextCommand > 0)
	        break;
	}
}

void Character::hitOpponent(Character* opponent, const char* curstate)
{
	cancellable = true;
	hit = true;
	hitstop = states[curstate].properties.hitstop;
	opponent->hitstop = states[curstate].properties.hitstop;
	opponent->actionable = false;
	if(opponent->xCollision)
		velocity.x = -(states[curstate].properties.pushbackVelocity.x * 0.0154f * states[curstate].properties.pushbackMultiplier);
	else
		opponent->velocity.x = -(states[curstate].properties.pushbackVelocity.x * 0.0154f * states[curstate].properties.pushbackMultiplier);
	opponent->velocity.y = states[curstate].properties.pushbackVelocity.y; //temp testing launches
	opponent->health -= states[curstate].properties.damage;
	opponent->hitstun = states[curstate].properties.hitstun;
	opponent->slowdown = states[curstate].properties.slowdown;
	opponent->firstFrameHit = true;
	handleEvent(GetCurrentState(), "HIT");
	opponent->SetState("CmnActHighGuardLoop");
	opponent->SetFrame(406);
	// handleEvent(currentState, "HIT_OR_GUARD");
}

bool Character::checkCollision(Character* opponent) //Clash is when two hitboxes touch, trade is when hitboxes hit each other chars hurtboxes at same time
{
	SetPushbox();
	if(hitboxes.count(currentFrame) > 0){
		hitboxActive = true;
		if(!hit && !opponent->firstFrameHit){
			if(opponent->hurtboxes.count(opponent->currentFrame) > 0){
				for(int i = 0; i < hitboxes[currentFrame].size(); i++){
					rect hitbox = ProcessRect(hitboxes[currentFrame][i]);
					hitbox.x += pos.x;
					hitbox.y += pos.y;
					for (int j = 0; j < opponent->hurtboxes[opponent->currentFrame].size(); ++j){
						rect hurtbox = opponent->ProcessRect(opponent->hurtboxes[opponent->currentFrame][j]);
						hurtbox.x += opponent->pos.x;
						hurtbox.y += opponent->pos.y;
						if(intersect(hitbox, hurtbox) && !hit){
							return true;
						}
					}
				}
			}
		}
	} else{
		hitboxActive = false;
	}
	return false;

}

bool Character::checkClash(Character* opponent)
{
	for(int i = 0; i < hitboxes[currentFrame].size(); i++){
		rect hitbox = ProcessRect(hitboxes[currentFrame][i]);
		hitbox.x += pos.x;
		hitbox.y += pos.y;
		for (int j = 0; j < opponent->hitboxes[opponent->currentFrame].size(); ++j){
			rect hitbox2 = opponent->ProcessRect(opponent->hurtboxes[opponent->currentFrame][j]);
			hitbox2.x += opponent->pos.x;
			hitbox2.y += opponent->pos.y;
			if(intersect(hitbox, hitbox2) && !hit){
				return true;
			}
		}
	}
	return false;
}

void Character::checkCommands()
{
	blocking = false;
	std::string curstate = GetCurrentState();

	if(characterState != 2){
		if(buttonMap["INPUT_ANY_DOWN"])
			characterState = 1;
		else
			characterState = 0;
	}

	if(curstate == "CmnActJumpPre"){
		if(buttonMap["INPUT_ANY_FORWARD"]){
			jumpDir = 1;
		}
		if(buttonMap["INPUT_ANY_BACK"]){
			jumpDir = 2;
		}
	}

	if(curstate == "CmnActStand"){
		if(buttonMap["INPUT_ANY_DOWN"]){
			SetState("CmnActStand2Crouch");
		}
		if(buttonMap["INPUT_HOLD_6"]){
			SetState("CmnActFWalk");
		} else if(buttonMap["INPUT_HOLD_4"]){
			SetState("CmnActBWalk");
		}
	}

	if(curstate == "CmnActStand2Crouch" || curstate == "CmnActCrouch"){
		if(buttonMap["INPUT_ANY_DOWN"]){

		} else{
			SetState("CmnActCrouch2Stand");
		}
	}

	if(curstate == "CmnActFWalk"){
		if(buttonMap["INPUT_HOLD_6"]){
			velocity.x = walkFSpeed;
		} else{
			SetState("CmnActStand");
			velocity.x = 0;
		}
	}

	if(curstate == "CmnActBWalk"){
		if(buttonMap["INPUT_HOLD_4"]){
			velocity.x = -walkBSpeed;
		} else{
			SetState("CmnActStand");
			velocity.x = 0;
		}
	}

	if(GetCurrentState() == "CmnActFDash"){
		if(buttonMap["INPUT_HOLD_6"]){
			velocity.x = std::round((velocity.x + fDashAccelSpeed - (velocity.x / fDashFriction)) * 1000.0) / 1000.0;
			if(velocity.x > dashMaxVelocity)
				velocity.x = dashMaxVelocity;
		} else
		{
			SetState("CmnActFDashStop");
		}
	}

	if(GetCurrentState() == "CmnActFDashStop"){
		velocity.x *= dashSkidDecay; //Dash skidding has no acceleration and is just friction til the end
		if(velocity.x < 0.2)
			velocity.x = 0;
	}

	if(buttonMap["INPUT_HOLD_4"] && std::find(validBlockingStates.begin(), validBlockingStates.end(), curstate) != validBlockingStates.end())
		blocking = true;

	for (const std::string& key : stateOrder) {

		if(states[key].properties.characterState != characterState)
			continue;

		if(key == "CmnActAirDash" && currentAirDashCount == 0)
			continue;

		bool gatling = false;
		bool cancel = false;
		bool kara = false;

		if(bbscriptFrameCount < karaFrames && states[key].properties.moveType == "SPECIAL" && states[curstate].properties.moveType == "NORMAL"){
			kara = true;
		}

		// if((std::find(states[curstate].stateTransitions.begin(), states[curstate].stateTransitions.end(), key) != states[curstate].stateTransitions.end())){
		// 	transition = true;
		// }

		bool self = (curstate == key);

		
		if((std::find(states[curstate].gatlingOptions.begin(), states[curstate].gatlingOptions.end(), key) != states[curstate].gatlingOptions.end())){
			gatling = true;
		} else if((std::find(states[curstate].cancelOptions.begin(), states[curstate].cancelOptions.end(), key) != states[curstate].cancelOptions.end())){
			cancel = true;
		} else if((std::find(states[curstate].whiffCancelOptions.begin(), states[curstate].whiffCancelOptions.end(), key) != states[curstate].whiffCancelOptions.end())){
			// gatling = false;
		} else{
			if(!recoveryState)
				continue;
		}

		State& value = states.at(key);
		if(value.properties.moveInput.size() == 2){

			if(buttonMap[value.properties.moveInput[0]])
			{
				std::cout << key << std::endl;
			    if(buttonMap[value.properties.moveInput[1]]){

					if(recoveryState || kara || (gatling && hit) || (!gatling && !hit && !cancel) || (cancel && cancellable)){
					    actionable = false;
					    handleEvent(GetCurrentState(), "BEFORE_EXIT");
						SetState(key);
						return;
					}
				}
			}
		}
		if(value.properties.moveInput.size() == 1){
			if(buttonMap[value.properties.moveInput[0]])
			{

			    if(recoveryState || kara || (gatling && hit) || (!gatling && !hit && !cancel) || (cancel && cancellable)){
			    	actionable = false;
			    	handleEvent(GetCurrentState(), "BEFORE_EXIT");
					SetState(key);
					return;
			    }
			}
		}
	}
}

void Character::updateScript(int tick, Character* opponent)
{
	pos = {static_cast<int>(round(pos.x)), static_cast<int>(round(pos.y))};	

	// if (!afterImages.empty()) {
	//     if (afterImages.size() > 8) {
	//         afterImages.pop_back();
	//     } else if (tick % 10 == 0) {
	//         afterImages.push_front(AfterImage(pos, spritesheet.GetFrame()));
	//     }
	// }

	blocking = false;
	std::string curstate = GetCurrentState();
	if(buttonMap["INPUT_HOLD_4"] && std::find(validBlockingStates.begin(), validBlockingStates.end(), curstate) != validBlockingStates.end())
		blocking = true;

	if(yCollision){
		if(!stateTouchedGround && stateLeftGround){
			FaceOpponent(opponent);
			handleEvent(GetCurrentState(), "TOUCH_GROUND");
			currentAirDashCount = airDashCount;
			stateTouchedGround = true;
			std::cout << "handled" << std::endl;
			if(landingStiffTime > 0)
				SetState("CmnActLandingStiff");
			else{
				SetState("CmnActJumpLanding");
			}
		}
		if(GetCurrentState() == "CmnActStand"){
			FaceOpponent(opponent);
		}
		stateLeftGround = false;
		// velocity.x = 0;
		// velocity.y = 0;
	} else {
		stateLeftGround = true;
	}

	if(isColliding(opponent)){
		if(velocity.x != 0 || pushFlag){
			if(centerPos.x < opponent->GetCenterPos().x)
				opponent->MoveX(((GetPushbox().x + width) - (opponent->pos.x + opponent->posOffset.x)) + 1);
			else
				opponent->MoveX((GetPushbox().x - (opponent->pos.x + opponent->posOffset.x + opponent->width)) - 1);
		}
	}

	// std::cout << health << std::endl;
	// for (const std::string& key : motionInputPriority) {
	// 	int sze = motionInputs[key].size();
	// 	for (size_t i = 0; i < sze; ++i) {
	// 		CommandSequence& cmd = motionInputs[key][i];

	// 		if(key == "INPUT_1080" && !buttonMap["INPUT_1080"] && !buttonMap["INPUT_720"]){
	// 			break;
	// 		}
	// 		if(key == "INPUT_720" && !buttonMap["INPUT_720"] && !buttonMap["INPUT_360"]){
	// 			break;
	// 		}
	// 		bool check = inputHandler->checkCommand(cmd);

	// 		if(check){
	// 			if(!buttonMap[key]){
	// 				for(size_t j = 0; j < sze; ++j){
	// 					if(j==i) continue;
	// 					motionInputs[key][j].commandNumber = 0;
	// 					motionInputs[key][j].commandTimer = 0;
	// 					motionInputs[key][j].executeTimer = 0;
	// 				}
	// 			}
	// 			buttonMap[key] = true;
	// 			break;
	// 		}
	// 		else{
	// 			buttonMap[key] = false;
	// 		}
	// 	}
	// }

	for (std::size_t i = 0; i < MOTIONS_1_SIZE; ++i) {
		_motions1[i].checkValidCommands(buttonMap, inputHandler);
	}

	for (std::size_t i = 0; i < MOTIONS_3_SIZE; ++i) {
		_motions3[i].checkValidCommands(buttonMap, inputHandler);
	}
	// _motions6[0].checkValidCommands(buttonMap, inputHandler);

	// if(inputHandler->checkCommand(_commands[0])){
	// 	std::cout << "dp" << std::endl;
	// 	buttonMap["INPUT_623"] = true;
	// } else{
	// 	buttonMap["INPUT_623"] = false;
	// }

	for (const auto& [key, value] : buttons) {
		if(inputHandler->checkCommand(buttons[key].ID, buttons[key].hold))
	    	buttonMap[key] = true;
	    else
	    	buttonMap[key] = false;
	}

	checkCommands();

	// if(slowdown > 0){
    // 	slowdown--;
    // 	if(slowdown % 2 == 0){
    // 		if(hitstop > 0)
    //     		hitstop--;
    // 		return;
    // 	}
    // }

	if(hitstop > 0) {
        hitstop--;
        return;
    } else {
    	cancellable = false;
    }

    if(hitstun > 0)
    	hitstun--;


    if(firstFrameHit){
		firstFrameHit = false;
    }

    handleEvent(GetCurrentState(), "IDLING");
    if(states[GetCurrentState()].eventHandlers.find("TIMER") != states[GetCurrentState()].eventHandlers.end()){
    	if(states[GetCurrentState()].eventHandlers["TIMER"].eventTrigger == bbscriptFrameCount){
    		handleEvent(GetCurrentState(), "TIMER");
    	}
    }
    if(GetCurrentState() == "CmnActLandingStiff"){
    	if(landingStiffTime > 0)
    		landingStiffTime--;
    	else{
    		SetState("CmnActStand");
    	}
    }
	executeCommands();


 	//DISPLACED OLDSUBROUTINE, FIND A HOME FOR IT LATER
 	if(GetCurrentState() == "CmnActHighGuardLoop"){
		if(hitstun > guardEndStart){
	    	// std::cout << hitstun << std::endl;
	    	// std::cout << currentState << std::endl;
			if(!firstFrameHit)
				velocity.x -= (highBlockstunDecay * velocity.x);
		} else{
				velocity = {0.0f, 0.0f};
		   		acceleration = {0.0f, 0.0f};
				SetState("CmnActHighGuardEnd");
		}
	}

	velocity.y += gravity;
	velocity.y += acceleration.y;
	velocity.x += acceleration.x;
	MoveX((velocity.x * sign * carriedMomentumPercentage * velocityXRate) + (accumulatedX*sign));
	MoveY((velocity.y * velocityYRate) + accumulatedY);
	accumulatedX = 0;
	accumulatedY = 0;
	velocity.x *= velocityXPercentEachFrame;
	velocity.y *= velocityYPercentEachFrame;

	if(pos.y >= 980){
		pos.y = 980;
		yCollision = true;
	}
	centerPos = pos + centerVec;

	// if(!firstFrame)
	bbscriptFrameCount++;
	firstFrame = false;
}

void Character::SetPushbox()
{
	if (currentFrame >= 0 && currentFrame < pushboxes.size()) {
		if(pushboxes[currentFrame].width != 0){
			pushbox = ProcessRect(pushboxes[currentFrame]);
			width = pushbox.width;
			height = pushbox.height;
			posOffset = glm::vec2(pushbox.x, pushbox.y);
		}
	}
}

void Character::SetFlipped(bool flop)
{
	sign = flop ? -1 : 1;
	spritesheet->SetFlipped(flop);
	flipped = flop;
}

rect Character::ProcessRect(const rect& r)
{
	rect result;
	if(flipped)
		result = {spritesheet->getAnchorPosition() - r.x - r.width, r.y, r.width, r.height};
	else
		result = {r.x, r.y, r.width, r.height};

	return result;

}

void Character::FaceOpponent(Character* opponent)
{
	if(centerPos.x > opponent->centerPos.x)
		SetFlipped(true);
	else if(centerPos.x < opponent->centerPos.x)
		SetFlipped(false);
}

void Character::draw(Renderer* renderer)
{
	glm::vec2 drawPosition = {0.0f, 0.0f};
	drawPosition = pos - posOffset - glm::vec2(width, height);
	// spritesheet.SetFrame(currentFrame);
	// spritesheet.pos = pos;
	// spritesheet.draw(renderer);
	int vertCrossWidth = 2;
	int vertCrossHeight = 40;
	int horiCrossWidth = 32;
	int horiCrossHeight = 2;
	renderer->DrawOutline(pos + posOffset, glm::vec2(width, height), 0, glm::vec4(pushboxColor, 1.0f), 1);
	renderer->DrawQuad(pos + posOffset + glm::vec2(width/2 - vertCrossWidth/2, height - vertCrossHeight/2), glm::vec2(vertCrossWidth, vertCrossHeight), 0, glm::vec4(1.0f));
	renderer->DrawQuad(pos + posOffset + glm::vec2(width/2 - horiCrossWidth/2, height - horiCrossHeight/2), glm::vec2(horiCrossWidth, horiCrossHeight), 0, glm::vec4(1.0f));
	// renderer->DrawQuad({pos.x + spritesheet.getCurrentOffset().x*2 + spritesheet.getCurrentSize().x*1.5 - 5, pos.y}, glm::vec2(5, 5), 0, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

	if(hurtboxes.count(currentFrame) > 0){
		for (int i = 0; i < hurtboxes[currentFrame].size(); ++i){
			rect hurtbox = ProcessRect(hurtboxes[currentFrame][i]);
			renderer->DrawOutline({pos.x + hurtbox.x, pos.y + hurtbox.y}, {hurtbox.width, hurtbox.height}, 0, hurtboxColor, 1);
		}
	}

	if(hitboxes.count(currentFrame) > 0){
		for (int i = 0; i < hitboxes[currentFrame].size(); ++i){
			rect hitbox = ProcessRect(hitboxes[currentFrame][i]);
			renderer->DrawOutline({pos.x + hitbox.x, pos.y + hitbox.y}, {hitbox.width, hitbox.height}, 0, glm::vec4(hitboxColor, 1.0f), 1);
		}
	}
}

void Character::draw(Renderer* renderer, Renderer* paletteRenderer, Texture& palette, Texture& shadowpalette)
{
	// drawPosition = pos - posOffset - glm::vec2(width, height);

	// for(int i = afterImages.size() - 1; i >= 0; i--){
	// 	spritesheet.pos = afterImages[i].pos;
	// 	spritesheet.SetFrame(afterImages[i].ID);
	//     float opacity = static_cast<float>(i) / (afterImages.size() - 1);
	//     spritesheet.color = {0.5f, 0.8f, 1.0f, 1.0f - opacity};
	// 	spritesheet.draw(paletteRenderer, palette);
	// }
	spritesheet->SetFrame(currentFrame);
	// spritesheet.color = glm::vec4(1.0f);


	spritesheet->pos = pos;
	// spritesheet->pos.y+=(height*0.6);
	// spritesheet->SetFlippedVert(true);
	// spritesheet->draw(paletteRenderer, shadowpalette);

	// spritesheet->pos.y-=(height*0.6);
	// spritesheet->SetFlippedVert(false);
	spritesheet->draw(paletteRenderer, palette);

	int vertCrossWidth = 2;
	int vertCrossHeight = 40;
	int horiCrossWidth = 32;
	int horiCrossHeight = 2;
	renderer->DrawOutline(pos + posOffset, glm::vec2(width, height), 0, glm::vec4(pushboxColor, 1.0f), 1);
	renderer->DrawQuad(centerPos + glm::vec2(-vertCrossWidth/2, -vertCrossHeight/2), glm::vec2(vertCrossWidth, vertCrossHeight), 0, glm::vec4(1.0f));
	renderer->DrawQuad(centerPos + glm::vec2(-horiCrossWidth/2, -horiCrossHeight/2), glm::vec2(horiCrossWidth, horiCrossHeight), 0, glm::vec4(1.0f));
	// renderer->DrawQuad(pos, glm::vec2(5, 5), 0, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	// renderer->DrawQuad({pos.x + spritesheet.getCurrentOffset().x*2 + spritesheet.getCurrentSize().x*1.5 - 5, pos.y}, glm::vec2(5, 5), 0, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

	// spritesheet.drawShadow(paletteRenderer, (centerPos.x - width/2), centerPos.y, (centerPos.x + width/2), centerPos.y, (centerPos.x - width), centerPos.y + 500,centerPos.x, centerPos.y + 500, 0);

	if(hurtboxes.count(currentFrame) > 0){
		for (int i = 0; i < hurtboxes[currentFrame].size(); ++i){
			rect hurtbox = ProcessRect(hurtboxes[currentFrame][i]);
			if(blocking)
				renderer->DrawOutline({pos.x + hurtbox.x, pos.y + hurtbox.y}, {hurtbox.width, hurtbox.height}, 0, blockingHighColor, 1);
			else
				renderer->DrawOutline({pos.x + hurtbox.x, pos.y + hurtbox.y}, {hurtbox.width, hurtbox.height}, 0, hurtboxColor, 1);
		}
	}

	if(hitboxes.count(currentFrame) > 0){
		for (int i = 0; i < hitboxes[currentFrame].size(); ++i){
			rect hitbox = ProcessRect(hitboxes[currentFrame][i]);
			renderer->DrawOutline({pos.x + hitbox.x, pos.y + hitbox.y}, {hitbox.width, hitbox.height}, 0, glm::vec4(hitboxColor, 1.0f), 1);
		}
	}
}