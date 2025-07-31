#ifndef SCRIPTABLE_H
#define SCRIPTABLE_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <deque>
#include <unordered_map>
#include <stack>
#include <glm/glm.hpp>

#include "Actor.h"
#include "Spritesheet.h"
#include "Util.h"

struct Properties {
    std::string moveType;
    int characterState;
    std::vector<std::string> moveInput;
    int hitstop = 0;
    int slowdown = 0;
    int hitstun = 0;
    int blockstun = 0;
    int damage = 0;
    int juggleLimit = 0;
    int juggleStart = 0; //Later add support for multihits where each hit results in a different juggle start.

    float proration = 1.0f;

    float hitAirPushbackY = 0.0f;
    glm::vec2 pushbackVelocity = {0.0f, 0.0f};
    float pushbackMultiplier = 1.0f;
};

struct Instruction {
    std::string command;
    std::vector<std::string> parameters;
    std::vector<Instruction> children;
    std::vector<Instruction> elseBlock;
};

struct EventHandler {
    std::vector<Instruction> instructions;
    unsigned int eventTrigger = 0;
};

struct State {
    std::string name;
    Properties properties;
    std::vector<Instruction> instructions;
    std::unordered_map<std::string, EventHandler> eventHandlers;
    std::unordered_map<std::string, size_t> labels;

    std::vector<std::string> gatlingOptions; //Guilty Gear style gatling system, can cancel any time after the move hits
    std::vector<std::string> cancelOptions; //Street Fighter style cancel system, can cancel only during hitstop after the move hits
    std::vector<std::string> whiffCancelOptions;
};

struct Subroutine {
    std::string name;
    std::vector<Instruction> instructions;
    std::unordered_map<std::string, EventHandler> eventHandlers;
};

class Scriptable {

public:
	Scriptable(){}

    std::unordered_map<std::string, std::function<void(const std::vector<std::string>&)>> commandMap;

	std::unordered_map<std::string, State> states;
    std::unordered_map<std::string, Subroutine> subroutines;

    std::vector<std::string> stateOrder;
    int currentState = 0;
    int sign = 0;
    std::unordered_map<int, std::string> stateNames;

    void initCommandMap();

    void SetFrame(const int frame);

    void executeInstruction(const Instruction& instr);

    void SetFlipped(bool flop);

    // virtual void exitState(int oldstate, int newstate) = 0; //Polymorphic, characters and objects have different exit cons

    void handleEvent(const std::string& stateName, const std::string& eventName) {
        if (states.find(stateName) != states.end()) {
            const auto& handlers = states[stateName].eventHandlers;
            auto it = handlers.find(eventName);
            if (it != handlers.end()) {
                for (const auto& inst : it->second.instructions) {
                    executeInstruction(inst);
                }
            }
        }
    }

    void mergeEventHandlers(State& state, const Subroutine& subroutine) {
        for (const auto& [event, handler] : subroutine.eventHandlers) {
            if (state.eventHandlers.find(event) == state.eventHandlers.end()) {
                state.eventHandlers[event] = handler;
            }
        }
    }

    std::string GetCurrentState()
    {
        return stateNames[currentState];
    }


	void loadScript(const std::string& filename) {
        std::ifstream file(filename);
        std::string line;

        bool inStateBlock = false;
        bool inAddBlock = false;
        bool inEventBlock = false;
        bool inSubroutine = false;
        std::string currentEvent;
        std::string currentStateBlock;
        std::string currentAddBlock;
        std::string currentSubroutine;
        Properties prop;

        std::stack<Instruction*> ifStack;

        std::stack<std::vector<Instruction>*> instructionStack;
        
        while (std::getline(file, line)) {
            size_t commentPos = line.find("//");
            if (commentPos != std::string::npos) {
                line = line.substr(0, commentPos);
            }

            if (line.empty()) continue;

            line.erase(std::remove(line.begin(),line.end(),' '),line.end());
            line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());

            if(line.find("addMove:") != std::string::npos){
                State newState;
                newState.name = line.substr(8);
                stateOrder.push_back(newState.name);
                states[newState.name] = newState;
                inAddBlock = true;
                inStateBlock = false;
                currentAddBlock = newState.name;
                currentStateBlock = "";
            } else if(line.find("endMove:") != std::string::npos){
                inStateBlock = false;
                inAddBlock = false;
                currentStateBlock = "";
                currentAddBlock = "";
            } else if(line.find("beginState:") != std::string::npos){
                inStateBlock = true;
                inAddBlock = false;
                currentStateBlock = line.substr(11);
                currentAddBlock = "";
                instructionStack.push(&states[currentStateBlock].instructions);
            } else if(line.find("endState:") != std::string::npos){
                inStateBlock = false;
                inAddBlock = false;
                currentStateBlock = "";
                currentAddBlock = "";
                instructionStack.pop();
            } else if(line.find("upon:") != std::string::npos){
                inEventBlock = true;
                EventHandler newHandler;
                currentEvent = line.substr(5);
                if(inSubroutine){
                    subroutines[currentSubroutine].eventHandlers.emplace(currentEvent, EventHandler{});
                    instructionStack.push(&subroutines[currentSubroutine].eventHandlers[currentEvent].instructions);
                } else{
                    states[currentStateBlock].eventHandlers.emplace(currentEvent, EventHandler{});
                    instructionStack.push(&states[currentStateBlock].eventHandlers[currentEvent].instructions);
                }
            } else if(line.find("endUpon") != std::string::npos){
                inEventBlock = false;
                currentEvent = "";
                instructionStack.pop();
            } else if (line.find("beginLabel:") != std::string::npos) {
                std::string labelName = line.substr(11);
                states[currentStateBlock].labels[labelName] = states[currentStateBlock].instructions.size(); //Bug with labels instruction being wrong index
                // std::cout << "Label created called: " << labelName << " at line: " << states[currentStateBlock].instructions.size();
            } else if(line.find("beginSubroutine:") != std::string::npos){
                inSubroutine = true;
                currentSubroutine = line.substr(16);
                instructionStack.push(&subroutines[currentSubroutine].instructions);
            } else if(line.find("endSubroutine:") != std::string::npos){
                inSubroutine = false;
                currentSubroutine = "";
                instructionStack.pop();
            } else if(line.find("ifOperation:") != std::string::npos){
                Instruction newIf;
                newIf.command = "ifOperation";

                size_t colonPos = line.find(':');
                if (colonPos == std::string::npos) continue;

                std::string paramsStr = line.substr(colonPos + 1);
                std::istringstream iss(paramsStr);
                std::string param;
                while (std::getline(iss, param, ',')) {
                    newIf.parameters.push_back(trim(param));
                }

                instructionStack.top()->push_back(newIf);

                Instruction* currentIf = &instructionStack.top()->back();
                ifStack.push(currentIf);

                instructionStack.push(&currentIf->children);

                std::cout << "Nesting into ifOperation with condition: " << currentIf->parameters[0]
                     << ", current depth = " << instructionStack.size() << std::endl;
            } else if(line.find("endIf:") != std::string::npos){

                if(!instructionStack.empty())
                    instructionStack.pop();
            } else if(line.find("else:") != std::string::npos){
                if(!ifStack.empty()){
                    std::cout  << instructionStack.size() << std::endl;
                    instructionStack.push(&ifStack.top()->elseBlock);
                }

            } else if(line.find("endElse:") != std::string::npos){
                if(!instructionStack.empty())
                    instructionStack.pop();
                if(!ifStack.empty())
                    ifStack.pop();
            } else {
                if(inStateBlock || inEventBlock || inSubroutine){

                    size_t colonPos = line.find(':');
                    if (colonPos == std::string::npos) continue; 

                    Instruction inst;
                    inst.command = trim(line.substr(0, colonPos));
                    
                    std::string paramsStr = line.substr(colonPos + 1);
                    std::istringstream iss(paramsStr);
                    std::string param;
                    while (std::getline(iss, param, ',')) {
                        inst.parameters.push_back(trim(param));
                    }

                    instructionStack.top()->push_back(inst);
                    // states[currentStateBlock].instructions.push_back(inst);
                    // std::cout << currentStateBlock << " " << states[currentStateBlock].instructions[states[currentStateBlock].instructions.size() - 1].command << std::endl;
                }

                if(inAddBlock){
                    size_t colonPos = line.find(':');
                    if (colonPos == std::string::npos) continue; 
                    std::string function = trim(line.substr(0, colonPos));
                    std::string paramsStr = line.substr(colonPos + 1);

                    std::cout << function << " " << paramsStr << std::endl;

                    if(function == "moveType"){
                        states[currentAddBlock].properties.moveType = paramsStr;

                    } else if(function == "characterState"){
                        if(paramsStr == "STANDING")
                            states[currentAddBlock].properties.characterState = 0;
                        else if(paramsStr == "CROUCHING")
                            states[currentAddBlock].properties.characterState = 1;
                        else if(paramsStr == "JUMPING")
                            states[currentAddBlock].properties.characterState = 2;
                    } else if(function == "moveInput"){
                        states[currentAddBlock].properties.moveInput.push_back(paramsStr);
                    }
                }
            }
        }
    }

protected:
	int currentFrame = 0;
    int currentLine = 0;
    int bbscriptFrameCount = 0;
    int framesUntilNextCommand = 0;

    bool recoveryState = false;
    bool hit = false;

    unsigned int airDashCount = 0;
    unsigned int forwardSuperJumpDistance = 0;
    unsigned int backwardSuperJumpDistance = 0;

    float accumulatedX = 0.0f;
    float accumulatedY = 0.0f;
    float jumpHeight = 0.0f;
    float initGravity = 0.0f;
    float gravity = 20.0f;
    float requestedShake = 0.0f;
    float carriedMomentumPercentage = 1.0f;
    float walkFSpeed = 7.0f;
    float walkBSpeed = 5.0f;
    float fDashFriction = 6.0f; // Friction for dashing
    float fDashAccelSpeed = 5.0f;
    float initDashFSpeed = 10.0f; // 13.1
    float dashMaxVelocity = 38.51f; // v cannot exceed 38.5
    float dashSkidDecay = 0.5f;
    float velocityXRate = 1.0f;
    float velocityYRate = 1.0f;
    float velocityXPercentEachFrame = 1.0f;
    float velocityYPercentEachFrame = 1.0f;
    float forwardJumpDistance = 0.0f;
    float backwardJumpDistance = 0.0f;

    glm::vec2 velocity = {0.0f, 0.0f};
    glm::vec2 acceleration = {0.0f, 0.0f};
};

#endif