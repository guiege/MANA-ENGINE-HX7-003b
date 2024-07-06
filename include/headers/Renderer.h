#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>

#include "VAO.h"
#include "VBO.h"
#include "Shader.h"
#include "Texture.h"
#include "Util.h"

const size_t MaxQuadCount = 1000; // FOR DEFAULT PERFORMANCE SET TO 1000
const size_t MaxVertexCount = MaxQuadCount * 4;
const size_t MaxIndexCount = MaxQuadCount * 6;
const size_t MaxTextures = 32;

struct Stats
{
	uint32_t DrawCount = 0;
	uint32_t QuadCount = 0;
};

struct RendererData
{
	GLuint QuadVA = 0;
	GLuint QuadVB = 0;
	GLuint QuadIB = 0;

	GLuint WhiteTexture = 0;
	uint32_t WhiteTextureSlot = 0;

	uint32_t IndexCount = 0;

	Vertex* QuadBuffer = nullptr;
	Vertex* QuadBufferPtr = nullptr;

	std::array<uint32_t, MaxTextures> TextureSlots;
	uint32_t TextureSlotIndex = 1;

	Stats RenderStats;
};

class Renderer
{
	public:
		Renderer(Shader &shader);

		void DrawIndexedTexture(Texture &texture, Texture &palette, const glm::vec2& position, const glm::vec2& size = glm::vec2(1.0f, 1.0f), float rotate = 0.0f, const glm::vec4& color  = glm::vec4(1.0f));

		void DrawIndexedTextureAtlas(Texture &texture, Texture &palette, const glm::vec2& clipPos, const glm::vec2& clipSize, bool rotated, const glm::vec2& position, const glm::vec4& color = glm::vec4(1.0f));

		void DrawTextureAtlas(Texture &texture, const glm::vec2& clipPos, const glm::vec2& clipSize, bool rotated, const glm::vec2& position, const glm::vec4& color = glm::vec4(1.0f));

		void DrawTexture(Texture &texture, const glm::vec2& position, const glm::vec2& size = glm::vec2(1.0f, 1.0f), float rotate = 0.0f, const glm::vec4& color = glm::vec4(1.0f));

		void DrawTexture(Texture *texture, const glm::vec2& position, const glm::vec2& size = glm::vec2(1.0f, 1.0f), float rotate = 0.0f, const glm::vec4& color = glm::vec4(1.0f));

		void DrawTexture(Texture &texture, float x1, float y1, float x2, float y2, float rotate, const glm::vec4& color = glm::vec4(1.0f));

		void DrawTexture(Texture &texture, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, const glm::vec4& color = glm::vec4(1.0f));

		void DrawTexture(Texture &texture, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float textureCropBottom, const glm::vec4& color = glm::vec4(1.0f));

		void DrawShadow(const glm::vec2& segmentA, const glm::vec2& segmentB, const glm::vec2& light_position, const glm::vec4& color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

		void DrawRect(const glm::vec2& position, const glm::vec2& size, float rotate, const glm::vec4& color = glm::vec4(1.0f));

		void Delete();

		void initBatchRenderData();

		bool isBatch = false;

		void DrawBatch(Texture &texture1, Texture &texture2);

		void BeginBatch();
		void EndBatch();
		void Flush();

		void DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		void DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color, const glm::vec3& cameraPosition, float parallaxFactor);
		void DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, uint32_t textureID);
		void DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, uint32_t textureID, const glm::vec3& cameraPosition, float parallaxFactor);
		void DrawOutline(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color, float lineThickness);
		void DrawQuadAtlas(Texture& texture, const glm::vec2& clipPos, const glm::vec2& clipSize, bool rotated, const glm::vec2& position, const glm::vec4& color);
		void DrawFlippedQuadAtlas(Texture& texture, const glm::vec2& clipPos, const glm::vec2& clipSize, bool rotated, const glm::vec2& position, bool flipX, bool flipY, const glm::vec4& color);

		void RotateQuad(const glm::vec2& position, const glm::vec2& size, float rotation, glm::vec2* rotatedPositions);


		const Stats& GetStats();
		void ResetStats();

		float approxsin(float x)
		{
			float t = x * 0.15915f;
			t = t - (int)t;

			return 20.785f * (t - 0.0f) * (t - 0.5f) * (t - 1.0f);
		}

	private:
		Shader& shader;
		VAO vao;
		VAO customVAO;

		RendererData s_Data;

		void initRenderData();
};

#endif