#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <bitset>
#include <map>
#include <algorithm>

struct Animation{
    Animation(std::vector<int> key, std::vector<int> frame)
    :keyframes(key), frames(frame)
    {}
    Animation(std::vector<int> key, std::vector<int> frame, bool rep)
    :keyframes(key), frames(frame), repeat(rep)
    {}
    Animation()
    {}
    std::vector<int> keyframes;
    std::vector<int> frames;
    int currentIndex = 0;
    bool repeat = false;
    bool finished = false;
};

struct Vertex
{
	glm::vec3 Position;
	glm::vec4 Color;
	glm::vec2 TexCoords;
	float TexIndex;
};

struct CommandSequence{

    int commandTimer = 0;

    int commandNumber = 0;

    int executeTimer = 0;

    int executionFrames = 0;

    std::vector<std::bitset<4>> commandList;

    std::vector<int> timingList;

    CommandSequence(const std::vector<std::bitset<4>> &commandList, const std::vector<int> &tL)
    :commandList(commandList), timingList(tL)
    {
        executionFrames = abs(tL[0]); 
    }

    CommandSequence()
    {}

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

struct rect
{
    int x, y, width, height;

    int type = 0;

    bool checkCollision(glm::vec2 pos)
    {
        return (pos.x >= x && pos.x <= x + width &&
                pos.y >= y && pos.y <= y + height);
    }

    float area() const
    {
        return width*height;
    }
};

struct TextureData
{
    std::string file;
    std::string name;
    bool alpha;
    int width, height, nrChannels;
    unsigned char* data;
    unsigned int Filter_Min = GL_NEAREST; // filtering mode if texture pixels < screen pixels
    unsigned int Filter_Max = GL_NEAREST; // filtering mode if texture pixels > screen pixels
};

template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

static std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t");
    if (std::string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, (last - first + 1));
}

int extractIntFromVal(const std::string& input);

bool intersect(const rect& r1, const rect& r2);

void writeHurtboxesToFile(const std::map<int, std::vector<rect>>& hurtboxes, const std::string& filename);

void readHurtboxesFromFile(std::map<int, std::vector<rect>>& hurtboxes, const std::string& filename);

void readHurtboxesFromFile(std::map<int, std::vector<rect>>& hurtboxes, std::map<int, std::vector<rect>>& hitboxes, std::vector<rect>& pushboxes, const std::string& filename);

void sortBoxesByArea(std::vector<rect>& boxes);

void sortAllBoxesInFrames(std::map<int, std::vector<rect>>& hurtboxes);
