#include "Spritesheet.h"

Spritesheet::Spritesheet(Texture& texture, const char* file, const float xpos, const float ypos, const float width, const float height, const float rot, glm::vec4 color)
: Sprite(texture, xpos, ypos, width, height, rot, color)
{
	std::ifstream ifs(file);
	json input = json::parse(ifs);

	sheetindex = input["frames"];

	SetFrame(0);

	anchorPosition = curClip.sourceX*2 + curClip.w*1.5;
}

void Spritesheet::SetFrame(const int frame)
{
	currentFrame = frame;

	json curFrame = sheetindex[currentFrame];

	json curFrameRect = curFrame["frame"];
	json source = curFrame["spriteSourceSize"];

	curClip.x = curFrameRect["x"];
	curClip.y = curFrameRect["y"];
	curClip.w = curFrameRect["w"];
	curClip.h = curFrameRect["h"];
	curClip.sourceX = source["x"];
	curClip.sourceY = source["y"];

	curClip.rotated = curFrame["rotated"];
}

glm::vec2 Spritesheet::getCurrentSize()
{
	return glm::vec2(curClip.w, curClip.h);
}

glm::vec2 Spritesheet::getCurrentOffset()
{
	return glm::vec2(curClip.sourceX, curClip.sourceY);
}

void Spritesheet::recalculateAnchor()
{
	anchorPosition = curClip.sourceX*2 + curClip.w*1.5;
}


void Spritesheet::draw(Renderer* renderer) 
{
	if(renderer->isBatch){
		if(flipped){
			renderer->DrawFlippedQuadAtlas(texture, glm::vec2(curClip.x, curClip.y), glm::vec2(curClip.w, curClip.h), curClip.rotated, pos + glm::vec2(anchorPosition - curClip.sourceX - curClip.w , curClip.sourceY), true, false, color);
			// DEBUG DRAW renderer->DrawQuad(pos + glm::vec2(anchorPosition - curClip.sourceX - curClip.w , curClip.sourceY), glm::vec2(curClip.w, curClip.h), 0.0f, glm::vec4(1.0f, 0.0f, 0.0f, 0.3f));
		}
		else{
			renderer->DrawQuadAtlas(texture, glm::vec2(curClip.x, curClip.y), glm::vec2(curClip.w, curClip.h), curClip.rotated, (pos + glm::vec2(curClip.sourceX, curClip.sourceY)), color);
			// DEBUG DRAW renderer->DrawQuad(pos + glm::vec2(curClip.sourceX, curClip.sourceY), glm::vec2(curClip.w, curClip.h), 0.0f, glm::vec4(1.0f, 0.0f, 0.0f, 0.3f));
		}

	}
	else{
		renderer->DrawTextureAtlas(texture, glm::vec2(curClip.x, curClip.y), glm::vec2(curClip.w, curClip.h), curClip.rotated, (pos + glm::vec2(curClip.sourceX, curClip.sourceY)), color);
	}
}

int Spritesheet::getLength()
{
	return (sheetindex.size() -1);
}