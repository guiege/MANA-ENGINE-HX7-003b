#pragma once

#include "imgui.h"

#include "GameState.h"
#include "PostProcessor.h"

struct Keyframe {
    int tick;
    float value;
};

class SplashState : public GameState
{
public:
	static SplashState* get();

	bool enter();
	bool exit();

	void update(float dt);
	void render();

private:
	static SplashState sSplashState;

	Renderer* batchRenderer;
	PostProcessor* post;

	int tm_splash_timer = 60;
	int tm_exit_timer = 120;

	std::vector<Keyframe> keyframes = {
	    {0, 0.0f},
	    {60, 1.0f},
	    {100, 1.0f},
	    {160, 0.0f},
	    {220, 1.0f},
	    {260, 1.0f},
	    {320, 0.0f},
	    {380, 1.0f},
	    {420, 1.0f},
	    {480, 0.0f},
	};

	float lerp(float a, float b, float t) {
	    return a + t * (b - a);
	}

	float calculateFadeFactor(int currentTick) {
	    if (keyframes.size() < 2) return 0.0f;
	    
	    auto nextKeyframe = std::lower_bound(keyframes.begin(), keyframes.end(), currentTick,
	        [](const Keyframe& kf, int tick) { return kf.tick < tick; });
	    
	    if (nextKeyframe == keyframes.end()) return keyframes.back().value;
	    if (nextKeyframe == keyframes.begin()) return nextKeyframe->value;
	    
	    auto prevKeyframe = nextKeyframe - 1;
	    
	    float t = static_cast<float>(currentTick - prevKeyframe->tick) / (nextKeyframe->tick - prevKeyframe->tick);
	    return lerp(prevKeyframe->value, nextKeyframe->value, t);
	}

	bool isAnyKeyPressed() {
	    for (int i = 0; i < 1024; ++i) {
	        if (Keys[i]) {
	            return true;
	        }
	    }
	    return false;
	}

	SplashState();
};