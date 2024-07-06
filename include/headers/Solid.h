#ifndef SOLID_H
#define SOLID_H

#include "Sprite.h"
#include "Util.h"
#include <math.h>
#include <algorithm>

class Actor;

class Solid : public Sprite {
public:

    Solid(Texture& texture, const float xpos, const float ypos, const float width, const float height, const float rot, std::vector<Actor*>& actors, glm::vec4 color = glm::vec4(1.0f))
        : Sprite(texture, xpos, ypos, width, height, rot, color), actors(actors), xRemainder(0), yRemainder(0) {}

    Solid(const float xpos, const float ypos, const float width, const float height, const float rot, std::vector<Actor*>& actors, glm::vec4 color = glm::vec4(1.0f))
        : Sprite(xpos, ypos, width, height, rot, color), actors(actors), xRemainder(0), yRemainder(0) {}

    void Move(float x, float y);
    // void setActors(std::vector<Actor*>& actors);

    bool collideWith(Actor* other);
    bool collideWith(Actor& other);
    bool collideWith(Actor* other, const glm::ivec2& position);

    std::vector<Actor*> GetAllRidingActors();

    bool collidable = true;

private:

    std::vector<Actor*>& actors;

    float xRemainder, yRemainder;

};

#endif