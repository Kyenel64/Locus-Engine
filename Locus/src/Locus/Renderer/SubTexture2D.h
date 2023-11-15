// --- SubTexture2D -----------------------------------------------------------
// Creates a 2D texture from a texture atlas. Doesn't hold an ID since the
//	subtexture doesnt actually hold its own texture but references the atlas.
#pragma once

#include <glm/glm.hpp>

#include "Locus/Renderer/Texture.h"

namespace Locus
{
	class SubTexture2D
	{
	public:
		SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max);
		~SubTexture2D() = default;

		const Ref<Texture2D> GetTexture() const { return m_Texture; }
		const glm::vec2* GetTexCoords() const { return m_TexCoords; }

		static Ref<SubTexture2D> CreateFromCoords(const Ref<Texture2D>& texture, const glm::vec2& spritePos, const glm::vec2& cellSize, const glm::vec2& spriteSize = { 1, 1 });

	private:
		Ref<Texture2D> m_Texture;

		glm::vec2 m_TexCoords[4];
	};
}
