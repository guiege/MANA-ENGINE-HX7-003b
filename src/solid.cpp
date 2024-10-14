#include "Actor.h"
#include "Solid.h"

void Solid::Move(float x, float y)
{
	xRemainder += x;
	yRemainder += y;
	int moveX = round(xRemainder);
	int moveY = round(yRemainder);
	if(moveX != 0 || moveY != 0)
	{
		std::vector<Actor*> riding = GetAllRidingActors();

		collidable = false;
		if(moveX != 0)
		{
			xRemainder -= moveX;
			pos.x += moveX;
			if(moveX > 0) // Carry right
			{
				for (int i = 0; i < actors.size(); i++){
					int cnt = count(riding.begin(), riding.end(), actors[i]);
					if(collideWith(actors[i]))
					{
						actors[i]->MoveX(((pos.x + width) - (actors[i]->pos.x + actors[i]->posOffset.x)) + 1);
					}
					else if(cnt > 0)
					{
						actors[i]->MoveX(moveX);
					}
				}
			}
			else // Carry left
			{
				for (int i = 0; i < actors.size(); i++){
					int cnt = count(riding.begin(), riding.end(), actors[i]);
					if(collideWith(actors[i]))
					{
						actors[i]->MoveX((pos.x - (actors[i]->pos.x + actors[i]->posOffset.x + actors[i]->width)) - 1);
					}
					else if(cnt > 0)
					{
						actors[i]->MoveX(moveX);
					}
				}
			}
		}

		if(moveY != 0)
		{
			yRemainder -= moveY;
			pos.y += moveY;
			if(moveY > 0) // Carry down
			{
				for (int i = 0; i < actors.size(); i++){
					int cnt = count(riding.begin(), riding.end(), actors[i]);
					if(collideWith(actors[i]))
					{
						actors[i]->MoveY(((pos.y + height) - (actors[i]->pos.y + actors[i]->posOffset.y)) + 1);
					}
					else if(cnt > 0)
					{
						actors[i]->MoveY(moveY);
					}
				}
			}
			else // Carry up
			{
				for (int i = 0; i < actors.size(); i++){
					int cnt = count(riding.begin(), riding.end(), actors[i]);
					if(collideWith(actors[i]))
					{
						actors[i]->MoveY((pos.y - ((actors[i]->pos.y + actors[i]->posOffset.y) + actors[i]->height)) - 1);
					}
					else if(cnt > 0)
					{
						actors[i]->MoveY(moveY);
					}
				}
			}
		}

		collidable = true;

	}

}

// void Solid::setActors(std::vector<Actor*>& actors)
// {
// 	this->actors = actors;
// }

bool Solid::collideWith(Actor& other)
{
	int otherX = (other.pos.x + other.posOffset.x);
	int otherY = (other.pos.y + other.posOffset.y);
	   // collision x-axis?
    bool collisionX = this->pos.x + this->width >= otherX &&
        otherX + other.width >= this->pos.x;
    // collision y-axis?
    bool collisionY = this->pos.y + this->height >= otherY &&
        otherY + other.height >= this->pos.y;
    // collision only if on both axes
    return collisionX && collisionY;
}

bool Solid::collideWith(Actor* other)
{
	int otherX = (other->pos.x + other->posOffset.x);
	int otherY = (other->pos.y + other->posOffset.y);
	   // collision x-axis?
    bool collisionX = this->pos.x + this->width >= otherX &&
        otherX + other->width >= this->pos.x;
    // collision y-axis?
    bool collisionY = this->pos.y + this->height >= otherY &&
        otherY + other->height >= this->pos.y;
    // collision only if on both axes
    return collisionX && collisionY;
}

bool Solid::collideWith(Actor* other, const glm::ivec2& position)
{
    // collision x-axis?
    bool collisionX = this->pos.x + this->width >= position.x &&
        position.x + other->width >= this->pos.x;
    // collision y-axis?
    bool collisionY = this->pos.y + this->height >= position.y &&
        position.y + other->height >= this->pos.y;
    // collision only if on both axes
    return collisionX && collisionY;
}


std::vector<Actor*> Solid::GetAllRidingActors()
{
	std::vector<Actor*> riding;
	for (int i = 0; i < actors.size(); i++){
		if(actors[i]->IsRiding(*this))
		{
			riding.push_back(actors[i]);
		}
	}
	return riding;
}