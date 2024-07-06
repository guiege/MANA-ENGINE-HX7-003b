#include "Character.h"

void Character::init()
{
	readHurtboxesFromFile(hurtboxes, hitboxes, pushboxes, "res/hitboxes/testBoxes.hbox");
	SetFrame(0);
}

void Character::animate(int tick)
{
	if (currentAnim.keyframes.empty()) return;

    if (currentAnim.currentIndex < currentAnim.keyframes.size() - 1 &&
        animCount >= currentAnim.keyframes[currentAnim.currentIndex + 1]) {
        currentAnim.currentIndex++;
    }

    if (currentAnim.currentIndex < currentAnim.frames.size()) {
        SetFrame(currentAnim.frames[currentAnim.currentIndex]);
    }

    if (currentAnim.repeat && animCount == currentAnim.keyframes.back()) {
        animCount = 0;
        currentAnim.currentIndex = 0;
    }

    std::cout << "Frame: " << animCount
              << ", Index: " << currentAnim.currentIndex 
              << ", Current Frame: " << currentAnim.frames[currentAnim.currentIndex]
              << ", Current Tick" << tick
              << std::endl;

    animCount++;
}

void Character::SetFrame(const int frame)
{
	currentFrame = frame;

	if(currentFrame > spritesheet.getLength()){
		currentFrame = 0;
	}
	if(currentFrame < 0){
		currentFrame = spritesheet.getLength();
	}
	SetPushbox();
}

void Character::SetPushbox()
{
	if (currentFrame >= 0 && currentFrame < pushboxes.size()) {
		rect pushbox = pushboxes[currentFrame];

		if(flipped)
			pushbox = ProcessRect(pushboxes[currentFrame]);

		width = pushbox.width;
		height = pushbox.height;
		posOffset = glm::vec2(pushbox.x, pushbox.y);
	}
}

void Character::SetFlipped(bool flop)
{
	spritesheet.SetFlipped(flop);
	flipped = flop;
	SetPushbox();
}

rect Character::ProcessRect(const rect& r)
{
	rect result;
	if(flipped)
		result = {spritesheet.getAnchorPosition() - r.x - r.width, r.y, r.width, r.height};
	else
		result = {r.x, r.y, r.width, r.height};

	return result;

}

void Character::PlayAnimation(const Animation& anim)
{
	currentAnim = anim;
    animCount = 0;
    currentAnim.currentIndex = 0;
    SetFrame(currentAnim.frames[0]);
}

void Character::draw(Renderer* renderer)
{
	drawPosition = pos - posOffset - glm::vec2(width, height);
	spritesheet.SetFrame(currentFrame);
	spritesheet.pos = pos;
	spritesheet.draw(renderer);
	int vertCrossWidth = 2;
	int vertCrossHeight = 40;
	int horiCrossWidth = 32;
	int horiCrossHeight = 2;
	renderer->DrawOutline(pos + posOffset, glm::vec2(width, height), 0, glm::vec4(pushboxColor, 1.0f), 1);
	renderer->DrawQuad(pos + posOffset + glm::vec2(width/2 - vertCrossWidth/2, height - vertCrossHeight/2), glm::vec2(vertCrossWidth, vertCrossHeight), 0, glm::vec4(1.0f));
	renderer->DrawQuad(pos + posOffset + glm::vec2(width/2 - horiCrossWidth/2, height - horiCrossHeight/2), glm::vec2(horiCrossWidth, horiCrossHeight), 0, glm::vec4(1.0f));
	// renderer->DrawQuad(pos, glm::vec2(5, 5), 0, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	// renderer->DrawQuad({pos.x + spritesheet.getCurrentOffset().x*2 + spritesheet.getCurrentSize().x*1.5 - 5, pos.y}, glm::vec2(5, 5), 0, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

	if(hurtboxes.count(currentFrame) > 0){
		for (int i = 0; i < hurtboxes[currentFrame].size(); ++i){
			rect hurtbox = ProcessRect(hurtboxes[currentFrame][i]);
			renderer->DrawOutline({pos.x + hurtbox.x, pos.y + hurtbox.y}, {hurtbox.width, hurtbox.height}, 0, glm::vec4(hurtboxColor, 1.0f), 1);
		}
	}

	if(hitboxes.count(currentFrame) > 0){
		for (int i = 0; i < hitboxes[currentFrame].size(); ++i){
			rect hitbox = ProcessRect(hitboxes[currentFrame][i]);
			renderer->DrawOutline({pos.x + hitbox.x, pos.y + hitbox.y}, {hitbox.width, hitbox.height}, 0, glm::vec4(hitboxColor, 1.0f), 1);
		}
	}
}