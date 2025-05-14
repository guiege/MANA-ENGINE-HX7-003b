#ifndef SPRITE_H
#define SPRITE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "Renderer.h"
#include "Texture.h"

class Sprite
{
	public:
		int width, height;
		float rot;
		bool flipped = false;
		bool hasTex;
		glm::vec2 pos = glm::vec2(0, 0);
		glm::vec4 color = glm::vec4(1.0f);

		Texture& texture;

		Sprite(Texture& texture, const float xpos, const float ypos, const float width, const float height, const float rot, glm::vec4 color = glm::vec4(1.0f))
		: texture(texture), pos(xpos, ypos), width(width), height(height), rot(rot), color(color), hasTex(true) {}

		Sprite(const float xpos, const float ypos, const float width, const float height, const float rot, glm::vec4 color = glm::vec4(1.0f))
		: texture(texture), pos(xpos, ypos), width(width), height(height), rot(rot), color(color), hasTex(false) {}
		
		// bool CheckCollision(Sprite* other);
  		// bool CheckCollision(Sprite* other, const glm::ivec2& position);
  		// bool CheckCollision(std::vector<Sprite>& sprites, const glm::ivec2& position);

  		void SetFlipped(bool flip);

		void draw(Renderer* renderer);

		void SetRotation(const float r);
};


#endif