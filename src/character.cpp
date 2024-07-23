#include "Character.h"

void Character::init()
{
	motionInputs["INPUT_236"] = CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.FORWARD}, {-1, 1}, 8);
	motionInputs["INPUT_214"] = CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.BACK}, {-1, 1}, 8);
	motionInputs["INPUT_623"] = CommandSequence({FK_Input_Buttons.FORWARD, FK_Input_Buttons.DOWN, FK_Input_Buttons.FORWARD}, {-1, 1, 1}, 8);
	motionInputs["INPUT_421"] = CommandSequence({FK_Input_Buttons.BACK, FK_Input_Buttons.DOWN, FK_Input_Buttons.BACK}, {-1, 1, 1}, 8);

	motionInputs["INPUT_22"] = CommandSequence({FK_Input_Buttons.DOWN, FK_Input_Buttons.DOWN}, {-1, 1}, 8);
	motionInputs["INPUT_44"] = CommandSequence({FK_Input_Buttons.BACK, FK_Input_Buttons.BACK}, {-1, 1}, 8);
	motionInputs["INPUT_66"] = CommandSequence({FK_Input_Buttons.FORWARD, FK_Input_Buttons.FORWARD}, {-1, 1}, 8);

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
		std::cout << "Setting sprite to " << params[0] << " for " << params[1] << " frames" << std::endl;
		SetFrame(stoi(params[0]));
		framesUntilNextCommand = stoi(params[1]);
	};

	commandMap["setCarriedMomentumPercentage"] = [this](const std::vector<std::string>& params) {
		if (params.empty()) return;
        std::cout << "Setting carried momentum percentage to " << params[0] << std::endl;
        framesUntilNextCommand = 0;
    };

    commandMap["physicsXImpulse"] = [this](const std::vector<std::string>& params) {
		if (params.empty()) return;
		std::cout << "Giving a x physics impulse of " << params[0] << std::endl;
		velocity.x = stoi(params[0]);
        framesUntilNextCommand = 0;
    };

    commandMap["physicsYImpulse"] = [this](const std::vector<std::string>& params) {
		if (params.empty()) return;
		std::cout << "Giving a y physics impulse of " << params[0] << std::endl;
		velocity.y = -stoi(params[0]);
        framesUntilNextCommand = 0;
    };

    commandMap["setGravity"] = [this](const std::vector<std::string>& params) {
		if (params.empty()) return;
		std::cout << "Setting gravity value to " << params[0] << std::endl;
		gravity = stoi(params[0]);
        framesUntilNextCommand = 0;
    };

    SetFlipped(false);
	start();
}

void Character::SetState(std::string state)
{
	bbscriptFrameCount = 0;
	framesUntilNextCommand = 0;
	currentLine = 0;
	lastCommandExecuted = 0;
	currentState = state;
}

void Character::runScript()
{
    if (framesUntilNextCommand > 0) {
        framesUntilNextCommand--;
        return;
    }

    while (currentLine < states[currentState].instructions.size()) {
        auto it = commandMap.find(states[currentState].instructions[currentLine].command);
        if (it != commandMap.end()) {
            it->second(states[currentState].instructions[currentLine].parameters);
            lastCommandExecuted = bbscriptFrameCount;
            currentLine++;

            if (framesUntilNextCommand > 0) {
                break;
            }
        } else {
            std::cout << "Unknown command: " << states[currentState].instructions[currentLine].command << std::endl;
            currentLine++;
        }
    }

    bbscriptFrameCount++;
}

void Character::scriptSubroutine(int tick, Character* opponent)
{

	if(yCollision){
		velocity.x = 0;
		velocity.y = 0;
	}

	for (const auto & [ key, value ] : states) {
		if(value.properties.moveInput.size() == 2){
		    if (!value.properties.moveInput.empty())
		    {

		        if(inputHandler->checkCommand(motionInputs[value.properties.moveInput[0]]))
		        {
		        	if(inputHandler->checkCommand(buttons[value.properties.moveInput[1]].ID, buttons[value.properties.moveInput[1]].hold))
		        	{
		        		SetState(key);
		        	}
		        }
		    }
		} else if(value.properties.moveInput.size() == 1){
			if(value.properties.moveInput[0].find("PRESS") != std::string::npos || value.properties.moveInput[0].find("HOLD") != std::string::npos){
				if(inputHandler->checkCommand(buttons[value.properties.moveInput[0]].ID, buttons[value.properties.moveInput[0]].hold))
				{
					SetState(key);
				}
			} else {
				if(inputHandler->checkCommand(motionInputs[value.properties.moveInput[0]]))
				{
					SetState(key);
				}
			}
		}
	}
	runScript();

	if(hitboxes.count(currentFrame) > 0){
		for (int i = 0; i < hitboxes[currentFrame].size(); ++i){
			rect hitbox = ProcessRect(hitboxes[currentFrame][i]);
			if(opponent->hurtboxes.count(opponent->currentFrame) > 0){
				for (int j = 0; j < opponent->hurtboxes[opponent->currentFrame].size(); ++j){
					rect hurtbox = opponent->ProcessRect(opponent->hurtboxes[opponent->currentFrame][j]);
					if(intersect(hitbox, hurtbox)){
						std::cout << "collision" << std::endl;
					}
				}
			}
		}
	}

    velocity.y += gravity;
    velocity.y += acceleration.y;
    velocity.x += acceleration.x;
	MoveX(velocity.x * sign);
	MoveY(velocity.y);
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
		rect pushbox = pushboxes[currentFrame];

		if(flipped)
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

// void Character::PlayAnimation(const Animation& anim)
// {
// 	currentAnim = anim;
// 	currentAnim.finished = false;
//     animCount = 0;
//     currentIndex = 0;
//     SetFrame(currentAnim.frames[0]);
// }

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
	drawPosition = pos - posOffset - glm::vec2(width, height);
	spritesheet.SetFrame(currentFrame);
	spritesheet.pos = pos;
	spritesheet.draw(paletteRenderer, palette);
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