#include "Scriptable.h"

void Scriptable::initCommandMap()
{
	commandMap["sprite"] = [this](const std::vector<std::string>& params) {
		if (params.empty()) return;
		SetFrame(stoi(params[0]));
		framesUntilNextCommand = stoi(params[1]);
	};

	commandMap["recoveryState"] = [this](const std::vector<std::string>& params) {
		recoveryState = true;
	};

	commandMap["airDashCount"] = [this](const std::vector<std::string>& params) {
		airDashCount = stoi(params[0]);
	};

	commandMap["callSubroutine"] = [this](const std::vector<std::string>& params) {
		auto it = subroutines.find(params[0]);
	    if (it != subroutines.end()) {
	        const Subroutine& sub = it->second;
	        mergeEventHandlers(states[GetCurrentState()], sub);
	        for (const Instruction& inst : sub.instructions) {
	            executeInstruction(inst);
	        }
	    }
	};

	commandMap["setCarriedMomentumPercentage"] = [this](const std::vector<std::string>& params) {
		carriedMomentumPercentage = stoi(params[0]) / (float)100;
    };

    commandMap["resetGravity"] = [this](const std::vector<std::string>& params) {
    	gravity = initGravity;
    };

    commandMap["addPositionX"] = [this](const std::vector<std::string>& params) {
        accumulatedX = stoi(params[0]) / (float)1000;
    };

    commandMap["addPositionY"] = [this](const std::vector<std::string>& params) {
        accumulatedY = stoi(params[0]) / (float)1000;
    };

    commandMap["physicsXImpulse"] = [this](const std::vector<std::string>& params) {
		if (params.empty()) return;
		velocity.x = stoi(params[0]) / (float)1000;
    };

    commandMap["physicsYImpulse"] = [this](const std::vector<std::string>& params) {
		if (params.empty()) return;
		velocity.y = -(stoi(params[0]) / (float)1000);
    };

    commandMap["setGravity"] = [this](const std::vector<std::string>& params) {
		if (params.empty()) return;
		gravity = stoi(params[0]) / (float)1000;
    };

   	commandMap["clearRegisteredUponCode"] = [this](const std::vector<std::string>& params) {
    };

    commandMap["setEventTrigger"] = [this](const std::vector<std::string>& params) {
        states[GetCurrentState()].eventHandlers[params[0]].eventTrigger = stoi(params[1]);
    };

    commandMap["gotoLabel"] = [this](const std::vector<std::string>& params) {
    	currentLine = states[GetCurrentState()].labels[params[0]];
    	executeInstruction(states[GetCurrentState()].instructions[currentLine]);
    };

    commandMap["addCancelOption"] = [this](const std::vector<std::string>& params) {
    	states[GetCurrentState()].cancelOptions.push_back(params[0]);
    };

    commandMap["addGatlingOption"] = [this](const std::vector<std::string>& params) {
    	states[GetCurrentState()].gatlingOptions.push_back(params[0]);
    };

    commandMap["addWhiffCancelOption"] = [this](const std::vector<std::string>& params) {
    	states[GetCurrentState()].whiffCancelOptions.push_back(params[0]);
    };

    commandMap["damage"] = [this](const std::vector<std::string>& params) {
    	states[GetCurrentState()].properties.damage = stoi(params[1]);
    };

    commandMap["hitstunAmount"] = [this](const std::vector<std::string>& params) {
    	states[GetCurrentState()].properties.hitstun = stoi(params[0]);
    };

    commandMap["blockstunAmount"] = [this](const std::vector<std::string>& params) {
    	states[GetCurrentState()].properties.blockstun = stoi(params[0]);
    };

    commandMap["hitStop"] = [this](const std::vector<std::string>& params) {
    	states[GetCurrentState()].properties.hitstop = stoi(params[0]);
    };

    commandMap["slowdown"] = [this](const std::vector<std::string>& params) {
    	states[GetCurrentState()].properties.slowdown = stoi(params[0]);
    };

    commandMap["cmn_screenshake"] = [this](const std::vector<std::string>& params) {
    	requestedShake = stoi(params[0]) / (float)100;
    };

    commandMap["pushbackX"] = [this](const std::vector<std::string>& params) {
    	states[GetCurrentState()].properties.pushbackMultiplier = stoi(params[0]) / 100.0f;
    };

    commandMap["hitAirPushbackY"] = [this](const std::vector<std::string>& params) {
    	states[GetCurrentState()].properties.pushbackMultiplier = stoi(params[0]) / 100.0f;
    };

    commandMap["attackLevel"] = [this](const std::vector<std::string>& params) {
	    int level = std::stoi(params[0]);
	    switch (level) {
	    	case 0:
	        	states[GetCurrentState()].properties.pushbackVelocity.x = 1250;
	        	break;
	       	case 1:
	        	states[GetCurrentState()].properties.pushbackVelocity.x = 1375;
	        	break;
	       	case 2:
	        	states[GetCurrentState()].properties.pushbackVelocity.x = 1500;
	        	break;	
	        case 3:
	        	states[GetCurrentState()].properties.pushbackVelocity.x = 1750;
	        	break;
	        case 4:
	        	states[GetCurrentState()].properties.pushbackVelocity.x = 2000;
	        	break;
	    }
    };

    commandMap["walkFSpeed"] = [this](const std::vector<std::string>& params) {
    	walkFSpeed = stoi(params[0]) / (float)1000;
    };

    commandMap["walkBSpeed"] = [this](const std::vector<std::string>& params) {
    	walkBSpeed = stoi(params[0]) / (float)1000;
    };

    commandMap["initDashFSpeed"] = [this](const std::vector<std::string>& params) {
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

    commandMap["velocityXRate"] = [this](const std::vector<std::string>& params) {
    	velocityXRate = (stoi(params[0]) / (float)100);
    };

    commandMap["velocityYRate"] = [this](const std::vector<std::string>& params) {
    	velocityYRate = (stoi(params[0]) / (float)100);
    };

    commandMap["velocityXPercent"] = [this](const std::vector<std::string>& params) {
    	velocity.x *= (stoi(params[0]) / (float)100);
    };

    commandMap["velocityYPercent"] = [this](const std::vector<std::string>& params) {
    	velocity.y *= (stoi(params[0]) / (float)100);
    };

    commandMap["velocityXPercentEachFrame"] = [this](const std::vector<std::string>& params) {
    	velocityXPercentEachFrame = stoi(params[0]) / (float)100;
    };

    commandMap["velocityYPercentEachFrame"] = [this](const std::vector<std::string>& params) {
    	velocityYPercentEachFrame = stoi(params[0]) / (float)100;
    };

    commandMap["forwardJumpDistance"] = [this](const std::vector<std::string>& params) {
    	forwardJumpDistance = stoi(params[0]) / (float)1000;
    };

    commandMap["backwardJumpDistance"] = [this](const std::vector<std::string>& params) {
    	backwardJumpDistance = stoi(params[0]) / (float)1000;
    };
    commandMap["jumpHeight"] = [this](const std::vector<std::string>& params) {
    	jumpHeight = stoi(params[0]) / (float)1000;
    };
    commandMap["initGravity"] = [this](const std::vector<std::string>& params) {
    	initGravity = stoi(params[0]) / (float)1000;
    };
    commandMap["forwardSuperJumpDistance"] = [this](const std::vector<std::string>& params) {
    	forwardSuperJumpDistance = stoi(params[0]);
    };

    commandMap["backwardSuperJumpDistance"] = [this](const std::vector<std::string>& params) {
    	forwardSuperJumpDistance = stoi(params[0]);
    };
    commandMap["superJumpHeight"] = [this](const std::vector<std::string>& params) {
    	forwardSuperJumpDistance = stoi(params[0]);
    };
    commandMap["superJumpGravity"] = [this](const std::vector<std::string>& params) {
    	forwardSuperJumpDistance = stoi(params[0]);
    };
}

void Scriptable::SetFrame(const int frame)
{
	currentFrame = frame;
}

void Scriptable::executeInstruction(const Instruction& instr)
{
    if (instr.command == "ifOperation") {
        // const auto& params = instr.parameters;
        // if (params.size() != 3) {
        //     std::cerr << "Invalid ifOperation parameters\n";
        //     return;
        // }

        bool result = false;

        std::string op = instr.parameters[0];
        std::string lhs = instr.parameters[1];
        std::string rhs = instr.parameters[2];

        int lhsVar = 0;
        int rhsVar = 0;

        if(lhs.find("Val") != std::string::npos)
        	lhsVar = extractIntFromVal(lhs);
        else if(lhs.find("SpeedY") != std::string::npos)
        	lhsVar = abs(velocity.y);
        else if(lhs.find("ActionTime") != std::string::npos)
        	lhsVar = bbscriptFrameCount + 1;

        if(rhs.find("Val") != std::string::npos)
        	rhsVar = extractIntFromVal(rhs);




        if(op == "IS_GREATER_OR_EQUAL"){
        	if(lhsVar >= rhsVar)
        		result = true;
        } else if(op == "IS_LESSER_OR_EQUAL"){
        	if(lhsVar <= rhsVar){
        		result = true;
        	}
        }

        // result = (instr.parameters[0] == "hello");

        if (result) {
        	if(instr.children.size() > 0){
	            for (const auto& child : instr.children) {
	                executeInstruction(child);
	            }
	        }
        } else{
        	if(instr.elseBlock.size() > 0){
        		for (const auto& elseinst : instr.elseBlock) {
                	executeInstruction(elseinst);
            	}
        	}
        }
    }
    else if(instr.command == "else"){

    } else {
        auto it = commandMap.find(instr.command);
        if (it != commandMap.end()) {
            it->second(instr.parameters);
        } else {
            std::cout << "Unknown command: " << instr.command << std::endl;
        }
    }
}