#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>

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

};



#endif