#include "Sprite.h"

void Sprite::draw(Renderer* renderer)
{
	if(renderer->isBatch)
	{
		if(hasTex){
			renderer->DrawQuad(pos, {width, height}, rot, texture.ID);
		}else
		{
			renderer->DrawQuad(pos, {width, height}, rot, color);
		}
	}
	else
	{
		if(hasTex){
			renderer->DrawTexture(texture, pos, {width, height}, rot, color);
		}else{
			renderer->DrawRect(pos, {width, height}, rot, color);
		}

	}
}

void Sprite::SetFlipped(bool flip)
{
	flipped = flip;
}

// bool Sprite::CheckCollision(Sprite* other) //AABB collision
// {
//     // collision x-axis?
//     bool collisionX = this->pos.x + this->width >= other->pos.x &&
//         other->pos.x + other->width >= this->pos.x;
//     // collision y-axis?
//     bool collisionY = this->pos.y + this->height >= other->pos.y &&
//         other->pos.y + other->height >= this->pos.y;
//     // collision only if on both axes
//     return collisionX && collisionY;
// }

// bool Sprite::CheckCollision(Sprite* other, const glm::ivec2& position)
// {
//     // collision x-axis?
//     bool collisionX = this->pos.x + this->width >= position.x &&
//         position.x + other->width >= this->pos.x;
//     // collision y-axis?
//     bool collisionY = this->pos.y + this->height >= position.y &&
//         position.y + other->height >= this->pos.y;
//     // collision only if on both axes
//     return collisionX && collisionY;
// }

// bool Sprite::CheckCollision(std::vector<Sprite>& sprites, const glm::ivec2& position)
// {
// 	for (auto& sprite : sprites)
// 	{
// 		if(sprite.CheckCollision(this, position))
// 		{
// 			return true;
// 		}
// 	}

// 	return false;
// }