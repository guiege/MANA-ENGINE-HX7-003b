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

#define MOTIONS_3_SIZE 2

struct AfterImage {

    glm::vec2 pos = {0.0f, 0.0f};
    int ID = 0;

    AfterImage(glm::vec2 p, int sprite)
    {
        pos = p;
        ID = sprite;
    }
};

template <std::size_t N>
struct MotionInput{
    CommandSequence validCommands[N];
    std::string name = "INPUT_623";


    MotionInput(){}

    MotionInput(const std::string& name_) 
    :name(name_) {}

    void checkValidCommands(std::unordered_map<std::string, bool>& buttonMap, InputHandler* inputHandler) {
        buttonMap[name] = false;
        for (std::size_t i = 0; i < N; ++i) {
            if(inputHandler->checkCommand(validCommands[i])){
                buttonMap[name] = true;
            }
        }
    }

};

class Character : public Actor, public Scriptable {
public:

    Character()
        : Actor(0,0,0,0,0),inputHandler(nullptr){}

    Character(InputHandler* handler, Texture& texture, const char* file, const float xpos, const float ypos, const float width, const float height, const float rot, glm::vec4 color = glm::vec4(1.0f))
        : Actor(xpos, ypos, 0, 0, rot), inputHandler(handler) 
    {
        spritesheet = new Spritesheet(texture, file, xpos, ypos, width, height, rot, color);
    }

    void init();

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

protected:
    //Components
    InputHandler* inputHandler;
    Spritesheet* spritesheet; // delete it please

    //Input Variables
    std::unordered_map<std::string, std::vector<CommandSequence>> motionInputs;
    std::vector<std::string> motionInputPriority;
    MotionInput<3> _motions3[MOTIONS_3_SIZE];
    // MotionInput<1> _motions1[3];
    // MotionInput<6> _motions6[1];

    std::unordered_map<std::string, Button> buttons;
    std::unordered_map<std::string, bool> buttonMap;

    std::deque<AfterImage> afterImages;

    //State Variables
    int characterState = 0; //3 potential states: STANDING, CROUCHING, JUMPING
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