#include "Spritefont.h"

Spritefont::Spritefont(const std::string& text, Texture& texture, const char* file, const float xpos, const float ypos, const float width, const float height, const float rot, glm::vec4 color)
: Spritesheet(texture, file, xpos, ypos, width, height, rot, color)
{
	codes = getAlphabetIndices(text);
	if(!animated){
		currentCodes = codes;
	}
}

void Spritefont::draw(Renderer* renderer) 
{
	if(renderer->isBatch){
		int oldx = pos.x;
		for(int i = 0; i < currentCodes.size(); i++){
			float yoffset = drawoffsetsin*4;
			if(i%2 == 0)
				yoffset = drawoffsetcos*4;
			if(currentCodes[i] >= 0){
				SetFrame(currentCodes[i]);
				renderer->DrawQuadAtlas(texture, glm::vec2(curClip.x, curClip.y), glm::vec2(curClip.w, curClip.h), curClip.rotated, pos + glm::vec2(anchorPosition - curClip.sourceX - curClip.w, curClip.sourceY + yoffset), color, glm::vec2(scale));
				pos.x += spacing*scale;
			}else{
				if(currentCodes[i] == -1)
					pos.x += spaceWidth*scale;
			}
		}
		pos.x = oldx;
	}
	else{
		// LMAO renderer->DrawTextureAtlas(texture, glm::vec2(curClip.x, curClip.y), glm::vec2(curClip.w, curClip.h), curClip.rotated, (pos + glm::vec2(curClip.sourceX, curClip.sourceY)), color);
	}
}

void Spritefont::playAnim(int sTick)
{
	index = 0;
	currentCodes.clear();
	startTick = sTick;
}

void Spritefont::update(int tick)
{
	int diff = tick-startTick;
	drawoffsetsin = sin(diff/10);
	drawoffsetcos = cos(diff/10);
	if(!animated || diff < 0){
		return;
	}
	int proc = (diff)%5;
	if(proc == 0){
		if(index < codes.size()){
			currentCodes.push_back(codes[index]);
			index++;
		}
	}
}