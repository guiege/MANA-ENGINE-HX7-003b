#ifndef SPRITEFONT_H
#define SPRITEFONT_H

#include "Spritesheet.h" 

class Spritefont : public Spritesheet {
public:
	std::vector<int> getAlphabetIndices(const std::string& str) {
	    std::vector<int> indices;

	    for (char c : str) {
	    	if (isspace(c)) {
	    		indices.push_back(-1);
	    	}
	        if (std::isalpha(c)) {
	            indices.push_back(std::tolower(c) - 'a');
	        }
	    }

	    return indices;
	}

	std::vector<int> codes;
	std::vector<int> currentCodes;
	unsigned int spacing = 5;
	unsigned int spaceWidth = 7;
	unsigned int scale = 10;

	unsigned int startTick = 20;
	unsigned int index = 0;

	bool animated = true;

	float drawoffsetsin = 0;
	float drawoffsetcos = 0;

	Spritefont(const std::string& text, Texture& texture, const char* file, const float xpos, const float ypos, const float width, const float height, const float rot, glm::vec4 color = glm::vec4(1.0f));

	void draw(Renderer* renderer);
	void update(int tick);
	void playAnim(int sTick);
};

#endif