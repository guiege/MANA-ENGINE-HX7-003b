#ifndef CHARACTER_H
#define CHARACTER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
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
};

struct Instruction {
    std::string command;
    std::vector<std::string> parameters;
};

struct State {
    std::string name;
    std::vector<Instruction> instructions;
    Properties properties;
};

struct Button {
    std::bitset<7> ID;
    bool hold;

    Button()
    {}

    Button(std::bitset<7> but, bool h)
    :ID(but), hold(h)
    {}
};

class Character : public Actor {
public:

    Character(InputHandler* handler, Texture& texture, const char* file, const float xpos, const float ypos, const float width, const float height, const float rot, 
        std::vector<Solid>& solids, glm::vec4 color = glm::vec4(1.0f))
        : Actor(xpos, ypos, 0, 0, rot, solids), inputHandler(handler), spritesheet(texture, file, xpos, ypos, width, height, rot, color) {}

    void init();

    int state = 0;

    int animID;
    int currentIndex = 0;

    virtual void start() = 0;

    virtual void update(int tick) = 0;

    virtual void enterState(int newstate, int oldstate) = 0;

    virtual void exitState(int oldstate, int newstate) = 0;

    void scriptSubroutine(int tick);
    void runScript();

    void draw(Renderer* renderer);
    void draw(Renderer* renderer, Renderer* paletteRenderer, Texture& palette);

    void SetFrame(const int frame);

    void SetFlipped(bool flop);

    bool GetFlipped(){
        return flipped;
    }

    void SetPushbox();

    void PlayAnimation(const Animation& anim);

    rect ProcessRect(const rect& r);

    int currentFrame = 0;

    Animation getAnimFromJson(json& file, const int index)
    {
        return Animation(file["animationList"][index]["keyframes"], file["animationList"][index]["frames"], file["animationList"][index]["repeat"]);
    }

    void loadScript(const std::string& filename) {
        std::ifstream file(filename);
        std::string line;

        bool inStateBlock = false;
        bool inAddBlock = false;
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
            } else {
                if(inStateBlock){
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
            }
        }
    }

protected:

    std::unordered_map<std::string, CommandSequence> motionInputs;
    std::unordered_map<std::string, Button> buttons;
    std::unordered_map<std::string, State> states;
    std::unordered_map<std::string, std::function<void(const std::vector<std::string>&)>> commandMap;

    int currentLine = 0;

    int bbscriptFrameCount = 0;
    int lastCommandExecuted = 0;
    int framesUntilNextCommand = 0;
    std::string currentState = "";

    Spritesheet spritesheet;

    glm::vec3 hurtboxColor = {10/(float)255, 185/(float)255, 230/(float)255};
    glm::vec3 pushboxColor = {130/(float)255, 1, 150/(float)255};
    glm::vec3 hitboxColor = {1, 175/(float)255, 175/(float)255};

    rect pushbox;

    std::map<int, std::vector<rect>> hurtboxes;
    std::map<int, std::vector<rect>> hitboxes;
    std::vector<rect> pushboxes;

    int animCount = 0;
    Animation currentAnim;

    bool flipped = false;
    int sign = 0;

    glm::vec2 drawPosition = {0.0f, 0.0f};

    InputHandler* inputHandler;


    //MOVEMENT VARIABLES
    glm::vec2 velocity = {0.0f, 0.0f};
    float gravity = 0;

};

#endif