#ifndef ACTOR_H
#define ACTOR_H

#include "Solid.h"
#include "Util.h"
#include <math.h>

class Actor{
public:

    int width, height;
    float rot;
    glm::vec2 pos = glm::ivec2(0, 0);
    glm::vec2 posOffset = glm::ivec2(0, 0);

    bool xCollision = false;
    bool yCollision = false;

    Actor(const int xpos, const int ypos, const int width, const int height, const float rot, std::vector<Solid>& solids);

    void MoveX(float amount);
    void MoveY(float amount);

    // void setSolids(std::vector<Solid>& solids);

    void draw(Renderer* renderer);

    bool CheckCollision(Solid& other);
    bool CheckCollision(std::vector<Solid>& solids, const glm::ivec2& position);

    bool IsRiding(Solid& solid);
    // void Squish();

private:

    std::vector<Solid>& solids;

    float xRemainder, yRemainder;

};

#endif