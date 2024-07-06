#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

struct Vertex
{
	glm::vec3 Position;
	glm::vec4 Color;
	glm::vec2 TexCoords;
	float TexIndex;
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
};

template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

void writeHurtboxesToFile(const std::map<int, std::vector<rect>>& hurtboxes, const std::string& filename);

void readHurtboxesFromFile(std::map<int, std::vector<rect>>& hurtboxes, const std::string& filename);

void readHurtboxesFromFile(std::map<int, std::vector<rect>>& hurtboxes, std::map<int, std::vector<rect>>& hitboxes, std::vector<rect>& pushboxes, const std::string& filename);

void sortBoxesByArea(std::vector<rect>& boxes);

void sortAllBoxesInFrames(std::map<int, std::vector<rect>>& hurtboxes);