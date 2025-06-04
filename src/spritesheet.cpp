#include "Spritesheet.h"

Spritesheet::Spritesheet(Texture& texture, const char* file, const float xpos, const float ypos, const float width, const float height, const float rot, glm::vec4 color)
: Sprite(texture, xpos, ypos, width, height, rot, color)
{
	std::ifstream ifs(file);
	json input = json::parse(ifs);

	sheetindex = input["frames"];

	SetFrame(0);

	anchorPosition = curClip.sourceX*2 + curClip.w*1.5;
	anchorPositionY = curClip.sourceY*2 + curClip.h*1.5;
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
	anchorPositionY = curClip.sourceY*2 + curClip.h*1.5;
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

void Spritesheet::draw(Renderer* renderer, Texture& palette) 
{
	//WOrks
	int flippedX = anchorPosition - curClip.sourceX - curClip.w;
	int flippedY = anchorPositionY - curClip.sourceY - curClip.h;
	if(curClip.rotated){
		flippedX = anchorPosition - curClip.sourceX;
		flippedY = anchorPositionY - curClip.sourceY - curClip.h*2;
	}
	if(renderer->isBatch){
	}
	else{
		if(flippedVert && flipped){
			renderer->DrawIndexedTextureAtlas(texture, palette, glm::vec2(curClip.x, curClip.y), glm::vec2(curClip.w, curClip.h), curClip.rotated, (pos + glm::vec2(flippedX, flippedY)), false,true,color);
		}else if(flippedVert){
			renderer->DrawIndexedTextureAtlas(texture, palette, glm::vec2(curClip.x, curClip.y), glm::vec2(curClip.w, curClip.h), curClip.rotated, (pos + glm::vec2(flippedX, flippedY)), true,true,color);
		} else if(flipped){
			renderer->DrawIndexedTextureAtlas(texture, palette, glm::vec2(curClip.x, curClip.y), glm::vec2(curClip.w, curClip.h), curClip.rotated, 
				pos + glm::vec2(anchorPosition - curClip.sourceX - curClip.w , curClip.sourceY), true,false,color);
		} else{
			renderer->DrawIndexedTextureAtlas(texture, palette, glm::vec2(curClip.x, curClip.y), glm::vec2(curClip.w, curClip.h), curClip.rotated, (pos + glm::vec2(curClip.sourceX, curClip.sourceY)), false,false, color);
			// void DrawTexture(Texture &texture, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float textureCropBottom, const glm::vec4& color = glm::vec4(1.0f));
			//shadowRenderer->DrawTexture(texture, testChar->pos.x, testChar->pos.y, 960, testChar->pos.y, 960 + (100 * skew), bottompos, (100 * skew), bottompos, 0.12, glm::vec4(0.0f, 0.0f, 0.0f, 0.5f));
		}
	}
}

void Spritesheet::drawShadow(Renderer* renderer, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float textureCropBottom, const glm::vec4& color) 
{
	if(renderer->isBatch){
	}
	else{
		renderer->DrawTexture(texture,x1,y1,x2,y2,x3,y3,x4,y4,textureCropBottom,color);
	}
}


void Spritesheet::drawclip(Renderer* renderer, const int x, const int y, const int w, const int h, bool rotated)
{
	if(renderer->isBatch){
		renderer->DrawQuadAtlas(texture, glm::vec2(x, y), glm::vec2(w, h), rotated, pos, color);
	}
	else{
		renderer->DrawTextureAtlas(texture, glm::vec2(x, y), glm::vec2(w, h), rotated, pos, color);
	}
}

int Spritesheet::getLength()
{
	return (sheetindex.size() -1);
}