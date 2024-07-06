#ifndef CHARACTER_H
#define CHARACTER_H

#include "Actor.h"
#include "Spritesheet.h"
#include "Util.h"
#include "Animation.h"

class Character : public Actor {
public:

    Character(Texture& texture, const char* file, const float xpos, const float ypos, const float width, const float height, const float rot, std::vector<Solid>& solids, glm::vec4 color = glm::vec4(1.0f))
        : Actor(xpos, ypos, 0, 0, rot, solids), spritesheet(texture, file, xpos, ypos, width, height, rot, color) {}

    void init();

    // virtual void update(int tick) = 0;

    void animate(int tick);

    void draw(Renderer* renderer);

    void SetFrame(const int frame);

    void SetFlipped(bool flop);

    void SetPushbox();

    void PlayAnimation(const Animation& anim);

    rect ProcessRect(const rect& r);

    int currentFrame = 0;

private:

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

    glm::vec2 drawPosition;

};

#endif