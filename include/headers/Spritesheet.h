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

    void drawShadow(Renderer* renderer, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float textureCropBottom, const glm::vec4& color = glm::vec4(1.0f));

    void drawclip(Renderer* renderer, const int x, const int y, const int w, const int h, bool rotated);

    int GetFrame()
    {
        return currentFrame;
    }

    void ChangeCurSourceX(const int newVal);

    void ChangeCurSourceY(const int newVal);

    void SetFrame(const int frame);

    void SetClipSize(const int width, const int height)
    {
        curClip.w = width;
        curClip.h = height;
    }

    void SetScale(glm::vec2 s)
    {
        scale = s;
    }

    glm::vec2 getCurrentSize();

    glm::vec2 getCurrentOffset();

    int getAnchorPosition(){return anchorPosition;}

    void recalculateAnchor();

    int getLength();

    int anchorPosition;
    int anchorPositionY;

protected:
    json input;
    const char* filePath;

    clip curClip;

    unsigned int currentFrame = 0;


    glm::vec2 scale = {1.0f, 1.0f};

};

#endif