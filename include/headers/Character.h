#ifndef CHARACTER_H
#define CHARACTER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <deque>
#include <unordered_map>
#include <stack>

#include "Actor.h"
#include "Scriptable.h"
#include "Spritesheet.h"
#include "Util.h"
#include "InputHandler.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

struct AfterImage {

    glm::vec2 pos = {0.0f, 0.0f};
    int ID = 0;

    AfterImage(glm::vec2 p, int sprite)
    {
        pos = p;
        ID = sprite;
    }
};

class Character : public Actor, public Scriptable {
public:

    Character(InputHandler* handler, Texture& texture, const char* file, const float xpos, const float ypos, const float width, const float height, const float rot, 
        std::vector<Solid>& solids, glm::vec4 color = glm::vec4(1.0f))
        : Actor(xpos, ypos, 0, 0, rot, solids), inputHandler(handler), spritesheet(texture, file, xpos, ypos, width, height, rot, color) {}

    void init();

    virtual void start() = 0;

    virtual void update(int tick) = 0;

    virtual void enterState(int newstate, int oldstate) = 0;

    virtual void exitState(int oldstate, int newstate) = 0;

    void updateScript(int tick, Character* opponent);
    void checkCommands();
    bool checkCollision(Character* opponent);
    bool checkClash(Character* opponent);
    void executeCommands();
    void exitState();

    void hitOpponent(Character* opponent, const char* curstate);

    void draw(Renderer* renderer);
    void draw(Renderer* renderer, Renderer* paletteRenderer, Texture& palette, Texture& shadowpalette);

    const glm::vec2& GetCenterPos() const {
        return centerPos;
    }

    // void SetCenterPos(const glm::vec2& new){

    // }

    void SetCenterPosY(const int& newY){
        pos.y = newY - centerVec.y;
    }

    bool isColliding(Character* opponent)
    {
        std::cout << GetPushbox().y << std::endl;
        return intersect(GetPushbox(), opponent->GetPushbox());
    }

    bool isActionable()
    {
        return actionable;
    }

    bool isHitboxActive()
    {
        return hitboxActive;
    }

    int GetHealth(){
        return health;
    }

    int GetHitstun(){
        return hitstun;
    }

    int GetHitstop(){
        return hitstop;
    }

    bool GetFlipped(){
        return flipped;
    }

    void SetPushFlag(bool push){
        pushFlag = push;
    }

    float GetRequestedShake(){
        return requestedShake;
    }

    void SetPushbox();

    void SetFlipped(bool flop);

    void SetRequestedShake(float shake){
        requestedShake = shake;
    }

    rect GetPushbox(){
        rect movedPushbox = pushbox;
        movedPushbox.x += pos.x;
        movedPushbox.y += pos.y;
        return movedPushbox;
    }

    void SetState(const std::string& state);

    rect ProcessRect(const rect& r);

    void FaceOpponent(Character* opponent);

    void insertMotionInput(const std::string& key, const std::vector<CommandSequence>& inputs){
        motionInputPriority.push_back(key);
        motionInputs[key] = inputs;
        buttonMap[key] = false;
    }

    std::string getHighestPriorityInput() {
        for (const auto& priorityKey : motionInputPriority) {
            if (std::find(motionInputBuffer.begin(), motionInputBuffer.end(), priorityKey) != motionInputBuffer.end()) {
                return priorityKey;
            }
        }
        return "";
    }

protected:
    //Components
    InputHandler* inputHandler;
    Spritesheet spritesheet;

    //Input Variables
    std::unordered_map<std::string, std::vector<CommandSequence>> motionInputs;
    std::vector<std::string> motionInputBuffer;
    std::vector<std::string> motionInputPriority;

    std::unordered_map<std::string, Button> buttons;
    std::unordered_map<std::string, bool> buttonMap;

    std::deque<AfterImage> afterImages;

    //State Variables
    std::string characterState = "STANDING"; //3 potential states: STANDING, CROUCHING, JUMPING
    //ADD PROXIMITY GUARD
    std::unordered_map<std::string, int> stateIDs;

    bool cancellable = false;
    bool blocking = false;

    //Animation Variables
    int guardEndStart = 9; //Frames that the guard end animation takes so it doesn't mess with hitstun(MAKE UNIVERSAL FIX FOR THIS LATER THANKS :))
    bool firstFrame = false;
    bool firstFrameHit = false;

    //Hitboxes
    std::map<int, std::vector<rect>> hurtboxes;
    std::map<int, std::vector<rect>> hitboxes;
    std::vector<rect> pushboxes;
    rect pushbox;
    glm::vec4 hurtboxColor = {10/(float)255, 185/(float)255, 230/(float)255, 1};
    glm::vec3 pushboxColor = {130/(float)255, 1, 150/(float)255};
    glm::vec3 hitboxColor = {1, 175/(float)255, 175/(float)255};
    glm::vec4 blockingHighColor = {0, 0, 1, 1};

    //Drawing Variables
    glm::vec2 drawPosition = {0.0f, 0.0f};
    glm::vec2 centerPos = {0.0f, 0.0f};
    glm::vec2 centerVec = {0.0f,0.0f};
    bool flipped = false;

    //Battle Variables
    unsigned int superJumpHeight = 0;
    unsigned int superJumpGravity = 0;
    unsigned int currentAirDashCount = 0;
    
    bool pushFlag = false;
    bool actionable = true;
    bool hitboxActive = false;
    bool stateTouchedGround = false;
    bool stateLeftGround = false;
    unsigned int landingStiffTime = 0;
    unsigned int health = 420;
    unsigned int hitstun = 0;
    unsigned int hitstop = 0;
    unsigned int slowdown = 0;
    int currentJuggle = 0;
    unsigned int jumpDir = 0;
    std::array<std::string, 2> validBlockingStates = { "CmnActStand", "CmnActBWalk"};

    int karaFrames = 3;

    //Parameters
    float highBlockstunDecay = .12;
};

#endif