#include "Actor.h"

Actor::Actor(const int xpos, const int ypos, const int width, const int height, const float rot, std::vector<Solid>& solids)
: pos(xpos, ypos), width(width), height(height), rot(rot), solids(solids), xRemainder(0), yRemainder(0)
{

}

void Actor::MoveX(float amount)
{
	xRemainder += amount;
	int move = round(xRemainder);
	if(move != 0)
	{
		xRemainder -= move;
		int sign = sgn(move);
		while(move != 0)
		{
			if(!CheckCollision(solids, glm::ivec2(this->pos.x + posOffset.x + sign, pos.y + posOffset.y)))
			{
				xCollision = false;
				pos.x += sign;
				move -= sign;
			}
			else
			{
				xCollision = true;
				// std::cout << "Collision!" << std::endl;
				break;
			}
			
		}
	}
}

void Actor::MoveY(float amount)
{
	yRemainder += amount;
	int move = round(yRemainder);
	if(move != 0)
	{
		yRemainder -= move;
		int sign = sgn(move);
		while(move != 0)
		{
			if(!CheckCollision(solids, glm::ivec2(pos.x + posOffset.x, pos.y + posOffset.y + sign)))
			{
				yCollision = false;
				pos.y += sign;
				move -= sign;
			}
			else
			{
				// std::cout << "Collision!" << std::endl;
				yCollision = true;
				break;
			}
			
		}
	}
}

// void Actor::setSolids(std::vector<Solid>& solids)
// {
// 	this->solids = solids;
// }


bool Actor::CheckCollision(Solid& other)
{
	// collision x-axis?
    bool collisionX = (pos.x + posOffset.x) + width >= other.pos.x &&
        other.pos.x + other.width >= (pos.x + posOffset.x);
    // collision y-axis?
    bool collisionY = (pos.y + posOffset.y) + height >= other.pos.y &&
        other.pos.y + other.height >= (pos.y + posOffset.y);
    // collision only if on both axes
    return collisionX && collisionY;
}

bool Actor::CheckCollision(std::vector<Solid>& solids, const glm::ivec2& position)
{
	for (auto& solid : solids)
	{
		if(solid.collidable && solid.collideWith(this, position))
		{
			return true;
		}
	}

	return false;
}

bool Actor::IsRiding(Solid& solid)
{
	bool collisionX = (pos.x + posOffset.x) + width >= solid.pos.x &&
        solid.pos.x + solid.width >= (pos.x + posOffset.x);

	if(collisionX && ((pos.y + posOffset.y) + height) == (solid.pos.y - 1))
	{
		return true;
	}
	return false;
}

void Actor::draw(Renderer* renderer)
{
	renderer->DrawRect(pos + posOffset, glm::vec2(width, height), rot, glm::vec4(1.0f));
	renderer->DrawRect(pos, glm::vec2(5, 5), 0, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
}