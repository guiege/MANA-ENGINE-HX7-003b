#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include "Sprite.h" 

#include <nlohmann/json.hpp>
using json = nlohmann::json;

struct clip {
    int x, y, w, h, sourceX, sourceY;
    bool rotated;
};

class Spritesheet : public Sprite {
public:

    Spritesheet(Texture& texture, const char* file, const float xpos, const float ypos, const float width, const float height, const float rot, glm::vec4 color = glm::vec4(1.0f));

    void draw(Renderer* renderer);

    void draw(Renderer* renderer, Texture& palette);

    void SetFrame(const int frame);

    glm::vec2 getCurrentSize();

    glm::vec2 getCurrentOffset();

    int getAnchorPosition(){return anchorPosition;}

    void recalculateAnchor();

    int getLength();

private:

    json sheetindex;

    clip curClip;

    unsigned int currentFrame = 0;

    int anchorPosition;

};

#endif