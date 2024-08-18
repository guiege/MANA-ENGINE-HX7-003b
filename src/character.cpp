#include "Character.h"

void Character::init()
{
	motionInputs["INPUT_236"] = CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.FORWARD}, {-1, 1}, 8);
	motionInputs["INPUT_214"] = CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.BACK}, {-1, 1}, 8);
	motionInputs["INPUT_623"] = CommandSequence({FK_Input_Buttons.FORWARD, FK_Input_Buttons.DOWN, FK_Input_Buttons.DOWN_FORWARD}, {-1, 1, 1}, 8);
	motionInputs["INPUT_421"] = CommandSequence({FK_Input_Buttons.BACK, FK_Input_Buttons.DOWN, FK_Input_Buttons.BACK}, {-1, 1, 1}, 8);

	motionInputs["INPUT_22"] = CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.DOWN}, {1, 1}, 8);
	motionInputs["INPUT_44"] = CommandSequence({FK_Input_Buttons.BACK, FK_Input_Buttons.BACK}, {1, 1}, 8);
	motionInputs["INPUT_66"] = CommandSequence({FK_Input_Buttons.FORWARD, FK_Input_Buttons.FORWARD}, {1, 1}, 8);

	motionInputs["INPUT_41236"] = CommandSequence({FK_Input_Buttons.BACK, FK_Input_Buttons.DOWN_BACK, FK_Input_Buttons.DOWN_FORWARD, FK_Input_Buttons.FORWARD}, {-1, 1, 1, 1}, 8);
	motionInputs["INPUT_63214"] = CommandSequence({FK_Input_Buttons.FORWARD, FK_Input_Buttons.DOWN_FORWARD, FK_Input_Buttons.DOWN_BACK, FK_Input_Buttons.BACK}, {-1, 1, 1, 1}, 8);

	motionInputs["INPUT_632146"] = CommandSequence({FK_Input_Buttons.FORWARD, FK_Input_Buttons.DOWN_FORWARD, FK_Input_Buttons.DOWN_BACK, FK_Input_Buttons.BACK, FK_Input_Buttons.FORWARD}, {-1, 1, 1, 1, 1}, 8);

	motionInputs["INPUT_2"] = CommandSequence({FK_Input_Buttons.DOWN}, {-1}, 8);
	motionInputs["INPUT_3"] = CommandSequence({FK_Input_Buttons.DOWN_FORWARD}, {-1}, 8);
	motionInputs["INPUT_4"] = CommandSequence({FK_Input_Buttons.BACK}, {-1}, 8);
	motionInputs["INPUT_6"] = CommandSequence({FK_Input_Buttons.FORWARD}, {-1}, 8);
	motionInputs["INPUT_8"] = CommandSequence({FK_Input_Buttons.UP}, {-1}, 8);
	motionInputs["INPUT_87"] = CommandSequence({FK_Input_Buttons.UP_BACK}, {-1}, 8);
	motionInputs["INPUT_89"] = CommandSequence({FK_Input_Buttons.UP_FORWARD}, {-1}, 8);

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

	commandMap["sprite"] = [this](const std::vector<std::string>& params) {
		if (params.empty()) return;
		// std::cout << "Setting sprite to " << params[0] << " for " << params[1] << " frames" << std::endl;
		SetFrame(stoi(params[0]));
		framesUntilNextCommand = stoi(params[1]) - 1;
	};

	commandMap["setCarriedMomentumPercentage"] = [this](const std::vector<std::string>& params) {
		carriedMomentumPercentage = stoi(params[0]) / (float)100;
        // std::cout << "Setting carried momentum percentage to " << params[0] << std::endl;
    };

    commandMap["physicsXImpulse"] = [this](const std::vector<std::string>& params) {
		if (params.empty()) return;
		// std::cout << "Giving a x physics impulse of " << params[0] << std::endl;
		velocity.x = stoi(params[0]) / (float)1000;
    };

    commandMap["physicsYImpulse"] = [this](const std::vector<std::string>& params) {
		if (params.empty()) return;
		// std::cout << "Giving a y physics impulse of " << params[0] << std::endl;
		velocity.y = -(stoi(params[0]) / (float)1000);
    };

    commandMap["setGravity"] = [this](const std::vector<std::string>& params) {
		if (params.empty()) return;
		// std::cout << "Setting gravity value to " << (stoi(params[0]) / (float)1000) << std::endl;
		gravity = stoi(params[0]) / (float)1000;
    };

   	commandMap["clearRegisteredUponCode"] = [this](const std::vector<std::string>& params) {
   		// std::cout << "clearing registered values" << std::endl;
   		velocity = {0.0f, 0.0f};
   		acceleration = {0.0f, 0.0f};
    };

    commandMap["gotoLabel"] = [this](const std::vector<std::string>& params) {
    	// std::cout << "Going to label: " << params[0] << " at line: " << states[currentState].labels[params[0]] << std::endl;
    	currentLine = states[GetCurrentState()].labels[params[0]] - 1;
    };

    commandMap["addCancelOption"] = [this](const std::vector<std::string>& params) {
    	states[GetCurrentState()].cancelOptions.push_back(params[0]);
    };

    commandMap["addGatlingOption"] = [this](const std::vector<std::string>& params) {
    	// std::cout << "Adding gatling option: " << params[0] << std::endl;
    	states[GetCurrentState()].gatlingOptions.push_back(params[0]);
    };

    commandMap["addWhiffCancelOption"] = [this](const std::vector<std::string>& params) {
    	// std::cout << "Adding whiff cancel option: " << params[0] << std::endl;
    	addWhiffCancelOption(params[0]);
    };

    commandMap["damage"] = [this](const std::vector<std::string>& params) {
    	// std::cout << "Setting damage to " << params[0] << std::endl;
    	states[GetCurrentState()].properties.damage = stoi(params[1]);
    };

    commandMap["hitstunAmount"] = [this](const std::vector<std::string>& params) {
    	// std::cout << "Setting " << params[0] << " frames of hitstun" << std::endl;
    	states[GetCurrentState()].properties.hitstun = stoi(params[0]);
    };

    commandMap["hitStop"] = [this](const std::vector<std::string>& params) {
    	// std::cout << "Setting " << params[0] << " frames of hitstop" << std::endl;
    	states[GetCurrentState()].properties.hitstop = stoi(params[0]);
    };

    commandMap["slowdown"] = [this](const std::vector<std::string>& params) {
    	states[GetCurrentState()].properties.slowdown = stoi(params[0]);
    };

    commandMap["cmn_screenshake"] = [this](const std::vector<std::string>& params) {
    	// std::cout << "Setting requested shake to " << (stoi(params[0]) / (float)100) << std::endl;
    	requestedShake = stoi(params[0]) / (float)100;
    };

    commandMap["pushbackX"] = [this](const std::vector<std::string>& params) {
    	states[GetCurrentState()].properties.pushbackMultiplier = stoi(params[0]) / 100.0f;
    };

    commandMap["cmn_AtkLv0"] = [this](const std::vector<std::string>& params) {
    	states[GetCurrentState()].properties.pushbackVelocity.x = 1250;
    	states[GetCurrentState()].properties.hitstop = 11;
    	states[GetCurrentState()].properties.hitstun = 12;
    	states[GetCurrentState()].properties.blockstun = 9;
    };

    commandMap["cmn_AtkLv1"] = [this](const std::vector<std::string>& params) {
    	states[GetCurrentState()].properties.pushbackVelocity.x = 1375;
    	states[GetCurrentState()].properties.hitstop = 12;
    	states[GetCurrentState()].properties.hitstun = 14;
    	states[GetCurrentState()].properties.blockstun = 11;
    };

    commandMap["cmn_AtkLv2"] = [this](const std::vector<std::string>& params) {
    	states[GetCurrentState()].properties.pushbackVelocity.x = 1500;
    	states[GetCurrentState()].properties.hitstop = 13;
    	states[GetCurrentState()].properties.hitstun = 16;
    	states[GetCurrentState()].properties.blockstun = 13;
    };

    commandMap["cmn_AtkLv3"] = [this](const std::vector<std::string>& params) {
    	states[GetCurrentState()].properties.pushbackVelocity.x = 1750;
    	states[GetCurrentState()].properties.hitstop = 14;
    	states[GetCurrentState()].properties.hitstun = 19;
    	states[GetCurrentState()].properties.blockstun = 16;
    };

    commandMap["cmn_AtkLv4"] = [this](const std::vector<std::string>& params) {
    	states[GetCurrentState()].properties.pushbackVelocity.x = 2000;
    	states[GetCurrentState()].properties.hitstop = 15;
    	states[GetCurrentState()].properties.hitstun = 21;
    	states[GetCurrentState()].properties.blockstun = 18;
    };

    commandMap["callSubroutine"] = [this](const std::vector<std::string>& params) {
    	callSubroutine(params[0]);
    };

    commandMap["walkFSpeed"] = [this](const std::vector<std::string>& params) {
    	walkFSpeed = stoi(params[0]) / (float)1000;
    };

    commandMap["walkBSpeed"] = [this](const std::vector<std::string>& params) {
    	walkBSpeed = stoi(params[0]) / (float)1000;
    };

    commandMap["initDashFSpeed"] = [this](const std::vector<std::string>& params) {
    	// std::cout << (stoi(params[0]) / (float)1000) << std::endl;
    	initDashFSpeed = stoi(params[0]) / (float)1000;
    };

    commandMap["fDashAccelSpeed"] = [this](const std::vector<std::string>& params) {
    	fDashAccelSpeed = stoi(params[0]) / (float)1000;
    };

	commandMap["fDashFriction"] = [this](const std::vector<std::string>& params) {
    	fDashFriction = stoi(params[0]);
    };

    commandMap["hit"] = [this](const std::vector<std::string>& params) {
    	hit = false;
    };


	// Add entries for motionInputs
	for (const auto& [key, value] : motionInputs) {
	    buttonMap[key] = false;
	}

	// Add entries for buttons
	for (const auto& [key, value] : buttons) {
	    buttonMap[key] = false;
	}

    SetFlipped(false);
	start();

	int count = 0;

    for (const auto& [key, value] : states) {
    	std::cout << key << std::endl;
	    stateIDs[key] = count;
	    stateNames[count] = key;
	    count++;
	}

	SetState("RoundInit");

	afterImages.push_front(AfterImage(pos, spritesheet.GetFrame()));
}

std::string Character::GetCurrentState()
{
	return stateNames[currentState];
}

void Character::SetState(const std::string& state)
{
	// std::cout << "setting state to: " << state << std::endl;
	bbscriptFrameCount = 0;
	framesUntilNextCommand = 0;
	currentLine = 0;
	lastCommandExecuted = 0;
	currentState = stateIDs[state];
	firstFrame = true;
	states[GetCurrentState()].gatlingOptions.clear();
	states[GetCurrentState()].whiffCancelOptions.clear();
	hit = false;
	cancellable = false;
	subroutines.clear();
	handleEvent(GetCurrentState(), "IMMEDIATE");

	if(GetCurrentState() == "CmnActStand")
		addWhiffCancelOption("cmnNandemoCancel");
	if(GetCurrentState() == "CmnActFWalk")
		addWhiffCancelOption("CmnActFDash");
}

void Character::executeCommands()
{
    if (framesUntilNextCommand > 0) {
        framesUntilNextCommand--;
        return;
    }

    while (currentLine < states[GetCurrentState()].instructions.size()) {
        auto it = commandMap.find(states[GetCurrentState()].instructions[currentLine].command);
        if (it != commandMap.end()) {
        	framesUntilNextCommand = 0;
            it->second(states[GetCurrentState()].instructions[currentLine].parameters);
            lastCommandExecuted = bbscriptFrameCount;
            currentLine++;

	        if (framesUntilNextCommand > 0) {
	            return;
	        }
        } else {
            std::cout << "Unknown command: " << states[GetCurrentState()].instructions[currentLine].command << std::endl;
            currentLine++;
        }
    }

	if (currentLine >= states[GetCurrentState()].instructions.size()) {
	    if (framesUntilNextCommand > 0) {
	        return; 
	    }
	    //current state animation has finished
		SetState("CmnActStand");
	}
}

void Character::checkCollision(Character* opponent)
{
	if(!hit && hitboxes.count(currentFrame) > 0){
		for (int i = 0; i < hitboxes[currentFrame].size(); ++i){
			rect hitbox = ProcessRect(hitboxes[currentFrame][i]);
			hitbox.x += pos.x;
			hitbox.y += pos.y;
			if(opponent->hurtboxes.count(opponent->currentFrame) > 0){
				for (int j = 0; j < opponent->hurtboxes[opponent->currentFrame].size(); ++j){
					rect hurtbox = opponent->ProcessRect(opponent->hurtboxes[opponent->currentFrame][j]);
					hurtbox.x += opponent->pos.x;
					hurtbox.y += opponent->pos.y;
					if(intersect(hitbox, hurtbox) && !hit){
						std::string curstate = GetCurrentState();
						handleEvent(GetCurrentState(), "HIT");
						// handleEvent(currentState, "HIT_OR_GUARD");
						cancellable = true;
						hit = true;
						opponent->SetState("CmnActHighGuardLoop");
						hitstop = states[curstate].properties.hitstop;
						opponent->hitstop = states[curstate].properties.hitstop;
						opponent->velocity.x = -(states[curstate].properties.pushbackVelocity.x * 0.0154f * states[curstate].properties.pushbackMultiplier);
						opponent->health -= states[curstate].properties.damage;
						opponent->hitstun = states[curstate].properties.hitstun;
						opponent->slowdown = states[curstate].properties.slowdown;
						opponent->firstFrameHit = true;
						break;
					}
				}
			}
		}
	}

}

void Character::checkCommands()
{

	for (const auto & [ key, value ] : states) {
		bool gatling = false;
		bool cancel = false;
		std::string curstate = GetCurrentState();
		if((std::find(states[curstate].gatlingOptions.begin(), states[curstate].gatlingOptions.end(), key) != states[curstate].gatlingOptions.end()) 
			|| (std::find(states[curstate].gatlingOptions.begin(), states[curstate].gatlingOptions.end(), "cmnNandemoCancel") != states[curstate].gatlingOptions.end())){
			gatling = true;
		} else if((std::find(states[curstate].cancelOptions.begin(), states[curstate].cancelOptions.end(), key) != states[curstate].cancelOptions.end()) 
			|| (std::find(states[curstate].cancelOptions.begin(), states[curstate].cancelOptions.end(), "cmnNandemoCancel") != states[curstate].cancelOptions.end())){
			cancel = true;
		} else if((std::find(states[curstate].whiffCancelOptions.begin(), states[curstate].whiffCancelOptions.end(), key) != states[curstate].whiffCancelOptions.end()) 
			|| (std::find(states[curstate].whiffCancelOptions.begin(), states[curstate].whiffCancelOptions.end(), "cmnNandemoCancel") != states[curstate].whiffCancelOptions.end())
			|| (bbscriptFrameCount < karaFrames && states[key].properties.moveType == "SPECIAL" && states[curstate].properties.moveType == "NORMAL")
			){
			gatling = false;
		} else{
			continue;
		}

		if(value.properties.moveInput.size() == 2){
			if (!value.properties.moveInput.empty())
			{
			    if(buttonMap[value.properties.moveInput[0]])
			    {
			    	if(buttonMap[value.properties.moveInput[1]]){
					    if((gatling && hit) || (!gatling && !hit && !cancel) || (cancel && cancellable)){
							SetState(key);
							break;
					    }
					}
			    }
			}
		}

		if(value.properties.moveInput.size() == 1){
			if(buttonMap[value.properties.moveInput[0]])
			{
			    if((gatling && hit) || (!gatling && !hit && !cancel) || (cancel && cancellable)){
					SetState(key);
					break;
			    }
			}
		}
	}
}

void Character::callSubroutine(const std::string& subroutine)
{
	if(subroutine == "cmnFDash"){
		velocity.x = initDashFSpeed;
	}
	subroutines.append(subroutine);
}

void Character::runSubroutines()
{
	if(subroutines.find("cmnHighGuard") != std::string::npos){
		if(hitstun > 0){
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

	if(subroutines.find("cmnFDashStop") != std::string::npos){
		velocity.x -= velocity.x * dashSkidDecay; //Dash skidding has no acceleration and is just friction til the end
		if(velocity.x < 0.2)
			velocity.x = 0;
	} else if(subroutines.find("cmnFDash") != std::string::npos){
		if(inputHandler->checkCommand(FK_Input_Buttons.FORWARD, true)){
			velocity.x = std::round((velocity.x + fDashAccelSpeed - (velocity.x / fDashFriction)) * 1000.0) / 1000.0;
			if(velocity.x > dashMaxVelocity)
				velocity.x = dashMaxVelocity;
		} else
		{
			SetState("CmnActFDashStop");
		}
	}
}

void Character::updateScript(int tick, Character* opponent)
{

	// if (!afterImages.empty()) {
	//     if (afterImages.size() > 8) {
	//         afterImages.pop_back();
	//     } else if (tick % 10 == 0) {
	//         afterImages.push_front(AfterImage(pos, spritesheet.GetFrame()));
	//     }
	// }

	if(isColliding(opponent)){
		std::cout << "pushbox collision" << std::endl;
		if(velocity.x != 0){
			if(sign > 0)
				opponent->MoveX(((GetPushbox().x + width) - (opponent->pos.x + opponent->posOffset.x)) + 1);
			else
				opponent->MoveX((GetPushbox().x - (opponent->pos.x + opponent->posOffset.x + opponent->width)) - 1);
		}
	}

	if(yCollision){
		handleEvent(GetCurrentState(), "TOUCH_GROUND");
		if(GetCurrentState() == "CmnActStand"){
			if(centerPos.x > opponent->centerPos.x)
			{
				SetFlipped(true);
			} else if(centerPos.x < opponent->centerPos.x)
			{
				SetFlipped(false);
			}
		}
		// velocity.x = 0;
		// velocity.y = 0;
	}

	if(GetCurrentState() == "CmnActStand"){
		if(inputHandler->checkCommand(FK_Input_Buttons.FORWARD, true)){
			SetState("CmnActFWalk");
		} else if(inputHandler->checkCommand(FK_Input_Buttons.BACK, true)){
			SetState("CmnActBWalk");
		}
	}

	if(GetCurrentState() == "CmnActFWalk"){
		if(inputHandler->checkCommand(FK_Input_Buttons.FORWARD, true)){
			velocity.x = walkFSpeed;
		} else{
			SetState("CmnActStand");
			velocity.x = 0;
		}
	}

	if(GetCurrentState() == "CmnActBWalk"){
		if(inputHandler->checkCommand(FK_Input_Buttons.BACK, true)){
			velocity.x = -walkBSpeed;
		} else{
			SetState("CmnActStand");
			velocity.x = 0;
		}
	}
	// std::cout << health << std::endl;
	for (const auto& [key, value] : motionInputs) {
		buttonMap[key] = false;
		if(inputHandler->checkCommand(motionInputs[key]))
	    	buttonMap[key] = true;
	}

	for (const auto& [key, value] : buttons) {
		buttonMap[key] = false; 
		if(inputHandler->checkCommand(buttons[key].ID, buttons[key].hold))
	    	buttonMap[key] = true;
	}

	checkCommands();

	if(slowdown > 0){
    	slowdown--;
    	if(slowdown % 2 == 0){
    		if(hitstop > 0)
        		hitstop--;
    		return;
    	}
    }

	if(hitstop > 0) {
        hitstop--;
        return;
    } else {
    	cancellable = false;
    }

    if(hitstun > 0)
    	hitstun--;

    if(firstFrameHit)
		firstFrameHit = false;

	executeCommands();
	update(tick);
	runSubroutines();
	checkCollision(opponent);

	velocity.y += gravity;
	   velocity.y += acceleration.y;
	   velocity.x += acceleration.x;
	MoveX(velocity.x * sign * carriedMomentumPercentage);
	MoveY(velocity.y);

	if(!firstFrame)
		bbscriptFrameCount++;
	firstFrame = false;
}

void Character::SetFrame(const int frame)
{
	currentFrame = frame;

	if(currentFrame > spritesheet.getLength()){
		currentFrame = 0;
	}
	if(currentFrame < 0){
		currentFrame = spritesheet.getLength();
	}
	SetPushbox();
}

void Character::SetPushbox()
{
	if (currentFrame >= 0 && currentFrame < pushboxes.size()) {
		pushbox = ProcessRect(pushboxes[currentFrame]);
		width = pushbox.width;
		height = pushbox.height;
		posOffset = glm::vec2(pushbox.x, pushbox.y);
	}
}

void Character::SetFlipped(bool flop)
{
	sign = flop ? -1 : 1;
	spritesheet.SetFlipped(flop);
	flipped = flop;
	SetPushbox();
}

rect Character::ProcessRect(const rect& r)
{
	rect result;
	if(flipped)
		result = {spritesheet.getAnchorPosition() - r.x - r.width, r.y, r.width, r.height};
	else
		result = {r.x, r.y, r.width, r.height};

	return result;

}

void Character::draw(Renderer* renderer)
{
	drawPosition = pos - posOffset - glm::vec2(width, height);
	spritesheet.SetFrame(currentFrame);
	spritesheet.pos = pos;
	spritesheet.draw(renderer);
	int vertCrossWidth = 2;
	int vertCrossHeight = 40;
	int horiCrossWidth = 32;
	int horiCrossHeight = 2;
	renderer->DrawOutline(pos + posOffset, glm::vec2(width, height), 0, glm::vec4(pushboxColor, 1.0f), 1);
	renderer->DrawQuad(pos + posOffset + glm::vec2(width/2 - vertCrossWidth/2, height - vertCrossHeight/2), glm::vec2(vertCrossWidth, vertCrossHeight), 0, glm::vec4(1.0f));
	renderer->DrawQuad(pos + posOffset + glm::vec2(width/2 - horiCrossWidth/2, height - horiCrossHeight/2), glm::vec2(horiCrossWidth, horiCrossHeight), 0, glm::vec4(1.0f));
	// renderer->DrawQuad(pos, glm::vec2(5, 5), 0, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	// renderer->DrawQuad({pos.x + spritesheet.getCurrentOffset().x*2 + spritesheet.getCurrentSize().x*1.5 - 5, pos.y}, glm::vec2(5, 5), 0, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

	if(hurtboxes.count(currentFrame) > 0){
		for (int i = 0; i < hurtboxes[currentFrame].size(); ++i){
			rect hurtbox = ProcessRect(hurtboxes[currentFrame][i]);
			renderer->DrawOutline({pos.x + hurtbox.x, pos.y + hurtbox.y}, {hurtbox.width, hurtbox.height}, 0, glm::vec4(hurtboxColor, 1.0f), 1);
		}
	}

	if(hitboxes.count(currentFrame) > 0){
		for (int i = 0; i < hitboxes[currentFrame].size(); ++i){
			rect hitbox = ProcessRect(hitboxes[currentFrame][i]);
			renderer->DrawOutline({pos.x + hitbox.x, pos.y + hitbox.y}, {hitbox.width, hitbox.height}, 0, glm::vec4(hitboxColor, 1.0f), 1);
		}
	}
}

void Character::draw(Renderer* renderer, Renderer* paletteRenderer, Texture& palette)
{
	// drawPosition = pos - posOffset - glm::vec2(width, height);

	// for(int i = afterImages.size() - 1; i >= 0; i--){
	// 	spritesheet.pos = afterImages[i].pos;
	// 	spritesheet.SetFrame(afterImages[i].ID);
	//     float opacity = static_cast<float>(i) / (afterImages.size() - 1);
	//     spritesheet.color = {0.5f, 0.8f, 1.0f, 1.0f - opacity};
	// 	spritesheet.draw(paletteRenderer, palette);
	// }

	spritesheet.color = glm::vec4(1.0f);
	spritesheet.pos = pos;
	spritesheet.SetFrame(currentFrame);
	spritesheet.draw(paletteRenderer, palette);
	int vertCrossWidth = 2;
	int vertCrossHeight = 40;
	int horiCrossWidth = 32;
	int horiCrossHeight = 2;
	centerPos = pos + posOffset + glm::vec2(width/2, height);
	renderer->DrawOutline(pos + posOffset, glm::vec2(width, height), 0, glm::vec4(pushboxColor, 1.0f), 1);
	renderer->DrawQuad(pos + posOffset + glm::vec2(width/2 - vertCrossWidth/2, height - vertCrossHeight/2), glm::vec2(vertCrossWidth, vertCrossHeight), 0, glm::vec4(1.0f));
	renderer->DrawQuad(pos + posOffset + glm::vec2(width/2 - horiCrossWidth/2, height - horiCrossHeight/2), glm::vec2(horiCrossWidth, horiCrossHeight), 0, glm::vec4(1.0f));
	// renderer->DrawQuad(pos, glm::vec2(5, 5), 0, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	// renderer->DrawQuad({pos.x + spritesheet.getCurrentOffset().x*2 + spritesheet.getCurrentSize().x*1.5 - 5, pos.y}, glm::vec2(5, 5), 0, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

	if(hurtboxes.count(currentFrame) > 0){
		for (int i = 0; i < hurtboxes[currentFrame].size(); ++i){
			rect hurtbox = ProcessRect(hurtboxes[currentFrame][i]);
			renderer->DrawOutline({pos.x + hurtbox.x, pos.y + hurtbox.y}, {hurtbox.width, hurtbox.height}, 0, glm::vec4(hurtboxColor, 1.0f), 1);
		}
	}

	if(hitboxes.count(currentFrame) > 0){
		for (int i = 0; i < hitboxes[currentFrame].size(); ++i){
			rect hitbox = ProcessRect(hitboxes[currentFrame][i]);
			renderer->DrawOutline({pos.x + hitbox.x, pos.y + hitbox.y}, {hitbox.width, hitbox.height}, 0, glm::vec4(hitboxColor, 1.0f), 1);
		}
	}
}