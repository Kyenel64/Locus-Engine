#pragma once

#include "Locus/Core/UUID.h"
#include "Locus/Renderer/Texture.h"

namespace Locus
{
	class TextureHandle
	{
	public:
		TextureHandle() : m_Path(std::filesystem::path()) {}
		TextureHandle(const std::filesystem::path& path) : m_Path(path) {}
		~TextureHandle() = default;

		Ref<Texture2D> Get() const;
		operator std::string() const { return m_Path.string(); }

		operator bool() const;
		bool operator==(const TextureHandle& other) const { return m_Path == other.m_Path; }
		bool operator!=(const TextureHandle& other) const { return m_Path != other.m_Path; }

		static TextureHandle Null;
	private:
		std::filesystem::path m_Path;
	};

	class TextureManager
	{
	public:
		TextureManager() = default;
		~TextureManager() = default;

		static void Init();
		
		static TextureHandle LoadTexture(const std::filesystem::path& path);
		
		static Ref<Texture2D> GetTexture(TextureHandle handle);
		static const std::unordered_map<TextureHandle, Ref<Texture2D>> GetTextures();

		static bool IsValid(TextureHandle handle);
	};
}

namespace std
{
	template<>
	struct hash<Locus::TextureHandle>
	{
		std::size_t operator()(const Locus::TextureHandle& handle) const
		{
			return hash<std::string>()(handle);
		}
	};
}