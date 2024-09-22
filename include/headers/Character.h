#ifndef CHARACTER_H
#define CHARACTER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <deque>
#include <unordered_map>

#include "Actor.h"
#include "Spritesheet.h"
#include "Util.h"
#include "InputHandler.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

struct Properties {
    std::string moveType;
    std::string characterState;
    std::vector<std::string> moveInput;
    int hitstop = 0;
    int slowdown = 0;
    int hitstun = 0;
    int blockstun = 0;
    int damage = 0;

    glm::vec2 pushbackVelocity = {0.0f, 0.0f};
    float pushbackMultiplier = 1.0f;
};

struct Instruction {
    std::string command;
    std::vector<std::string> parameters;
};

struct EventHandler {
    std::string event;
    std::vector<Instruction> instructions;
};

struct State {
    std::string name;
    Properties properties;
    std::vector<Instruction> instructions;
    std::vector<EventHandler> eventHandlers;
    std::unordered_map<std::string, size_t> labels;

    std::vector<std::string> gatlingOptions; //Guilty Gear style gatling system, can cancel any time after the move hits
    std::vector<std::string> cancelOptions; //Street Fighter style cancel system, can cancel only during hitstop after the move hits
    std::vector<std::string> whiffCancelOptions;
};

struct AfterImage {

    glm::vec2 pos = {0.0f, 0.0f};
    int ID = 0;

    AfterImage(glm::vec2 p, int sprite)
    {
        pos = p;
        ID = sprite;
    }
};

class Character : public Actor {
public:

    Character(InputHandler* handler, Texture& texture, const char* file, const float xpos, const float ypos, const float width, const float height, const float rot, 
        std::vector<Solid>& solids, glm::vec4 color = glm::vec4(1.0f))
        : Actor(xpos, ypos, 0, 0, rot, solids), inputHandler(handler), spritesheet(texture, file, xpos, ypos, width, height, rot, color) {}

    void init();

    int sign = 0;

    virtual void start() = 0;

    virtual void update(int tick) = 0;

    virtual void enterState(int newstate, int oldstate) = 0;

    virtual void exitState(int oldstate, int newstate) = 0;

    void updateScript(int tick, Character* opponent);
    void runSubroutines();
    void checkCommands();
    bool checkCollision(Character* opponent);
    void executeCommands();

    void hitOpponent(Character* opponent, const char* curstate);

    void draw(Renderer* renderer);
    void draw(Renderer* renderer, Renderer* paletteRenderer, Texture& palette);

    void SetFrame(const int frame);

    void SetFlipped(bool flop);

    const glm::vec2& GetCenterPos() const {
        return centerPos;
    }

    bool isColliding(Character* opponent)
    {
        return intersect(GetPushbox(), opponent->GetPushbox());
    }

    int GetHealth(){
        return health;
    }

    bool GetFlipped(){
        return flipped;
    }

    float GetRequestedShake(){
        return requestedShake;
    }

    void SetRequestedShake(float shake){
        requestedShake = shake;
    }

    rect GetPushbox(){
        rect movedPushbox = pushbox;
        movedPushbox.x += pos.x;
        movedPushbox.y += pos.y;
        return movedPushbox;
    }

    void SetPushbox();

    std::string GetCurrentState();

    void SetState(const std::string& state);

    void addWhiffCancelOption(const std::string& state){
        states[GetCurrentState()].whiffCancelOptions.push_back(state);
    }

    void callSubroutine(const std::string& subroutine);

    void clearSubroutines(){
        subroutines = "";
    }

    rect ProcessRect(const rect& r);

    void handleEvent(const std::string& stateName, const std::string& eventName) {
        if (states.find(stateName) != states.end()) {
            for (const auto& handler : states[stateName].eventHandlers) {
                if (handler.event == eventName) {
                    for (const auto& inst : handler.instructions) {
                        if (commandMap.find(inst.command) != commandMap.end()) {
                            commandMap[inst.command](inst.parameters);
                        } else {
                            std::cout << "Unknown command: " << inst.command << std::endl;
                        }
                    }
                }
            }
        }
    }

    void loadScript(const std::string& filename) {
        std::ifstream file(filename);
        std::string line;

        bool inStateBlock = false;
        bool inAddBlock = false;
        bool inEventBlock = false;
        std::string currentEvent;
        std::string currentStateBlock;
        std::string currentAddBlock;
        Properties prop;
        
        while (std::getline(file, line)) {
            size_t commentPos = line.find("//");
            if (commentPos != std::string::npos) {
                line = line.substr(0, commentPos);
            }

            if (line.empty()) continue;

            line.erase(std::remove(line.begin(),line.end(),' '),line.end());

            if(line.find("addMove:") != std::string::npos){
                State newState;
                newState.name = line.substr(8);
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
            } else if(line.find("endState:") != std::string::npos){
                inStateBlock = false;
                inAddBlock = false;
                currentStateBlock = "";
                currentAddBlock = "";
            } else if(line.find("upon:") != std::string::npos){
                inEventBlock = true;
                EventHandler newHandler;
                newHandler.event = line.substr(6);
                states[currentStateBlock].eventHandlers.push_back(newHandler);
            } else if(line.find("endUpon") != std::string::npos){
                inEventBlock = false;
                currentEvent = "";
            } else if (line.find("beginLabel:") != std::string::npos) {
                std::string labelName = line.substr(12);
                states[currentStateBlock].labels[labelName] = states[currentStateBlock].instructions.size();
                // std::cout << "Label created called: " << labelName << " at line: " << states[currentStateBlock].instructions.size();
            } else {
                if(inStateBlock && !inEventBlock){
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

                    states[currentStateBlock].instructions.push_back(inst);
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
                        states[currentAddBlock].properties.characterState = paramsStr;

                    } else if(function == "moveInput"){
                        states[currentAddBlock].properties.moveInput.push_back(paramsStr);
                    }
                }

                if(inEventBlock){
                    Instruction inst;
                    size_t colonPos = line.find(':');
                    if (colonPos != std::string::npos) {
                        inst.command = trim(line.substr(0, colonPos));
                        std::string paramsStr = line.substr(colonPos + 1);
                        std::istringstream iss(paramsStr);
                        std::string param;
                        while (std::getline(iss, param, ',')) {
                            inst.parameters.push_back(trim(param));
                        }
                        states[currentStateBlock].eventHandlers.back().instructions.push_back(inst);
                        }
                    }
                }
            }
        }

protected:
    //Components
    InputHandler* inputHandler;
    Spritesheet spritesheet;

    //Input Variables
    std::unordered_map<std::string, CommandSequence> motionInputs;
    std::unordered_map<std::string, Button> buttons;
    std::unordered_map<std::string, bool> buttonMap;

    std::deque<AfterImage> afterImages;

    //State Variables
    std::unordered_map<std::string, int> stateIDs;
    std::unordered_map<int, std::string> stateNames;

    std::unordered_map<std::string, State> states;
    std::unordered_map<std::string, std::function<void(const std::vector<std::string>&)>> commandMap;
    bool cancellable = false;
    bool hit = false;

    bool blocking = false;

    //Animation Variables
    int currentFrame = 0;
    int currentLine = 0;
    int bbscriptFrameCount = 0;
    int lastCommandExecuted = 0;
    int framesUntilNextCommand = 0;
    bool firstFrame = false;
    bool firstFrameHit = false;

    //Hitboxes
    std::map<int, std::vector<rect>> hurtboxes;
    std::map<int, std::vector<rect>> hitboxes;
    std::vector<rect> pushboxes;
    rect pushbox;
    glm::vec3 hurtboxColor = {10/(float)255, 185/(float)255, 230/(float)255};
    glm::vec3 pushboxColor = {130/(float)255, 1, 150/(float)255};
    glm::vec3 hitboxColor = {1, 175/(float)255, 175/(float)255};
    glm::vec3 blockingHighColor = {0, 0, 1};

    //Drawing Variables
    glm::vec2 drawPosition = {0.0f, 0.0f};
    glm::vec2 centerPos = {0.0f, 0.0f};
    bool flipped = false;

    //Battle Variables
    unsigned int health = 420;
    unsigned int hitstun = 0;
    unsigned int hitstop = 0;
    unsigned int slowdown = 0;
    int currentState = 0;
    std::string subroutines = "";

    //Movement Variables
    glm::vec2 velocity = {0.0f, 0.0f};
    glm::vec2 acceleration = {0.0f, 0.0f};
    float gravity = 20.0f; //gravity value varys by character
    float carriedMomentumPercentage = 1.0f;
    float walkFSpeed = 7.0f;
    float walkBSpeed = 5.0f;
    float fDashFriction = 6.0f; // Friction for dashing
    float fDashAccelSpeed = 5.0f;
    float initDashFSpeed = 10.0f; // 13.1
    float dashMaxVelocity = 38.51f; // v cannot exceed 38.5
    float dashSkidDecay = 0.25f;

    float velocityXPercentEachFrame = 1.0f;
    float velocityYPercentEachFrame = 1.0f;

    int karaFrames = 2;
    float requestedShake = 0.0f;

    //Parameters
    float highBlockstunDecay = .12;



};

#endif