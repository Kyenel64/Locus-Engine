#pragma once

#include "Locus/Core/UUID.h"
#include "Locus/Renderer/Texture.h"

namespace Locus
{
	class TextureHandle
	{
	public:
		TextureHandle() : Handle(UUID()) {}
		TextureHandle(UUID uuid) : Handle(uuid) {}
		~TextureHandle() = default;

		operator uint64_t() const { return (uint64_t)Handle; }
		operator bool() const;
		bool operator==(const TextureHandle& other) const { return Handle == other.Handle; }
		bool operator!=(const TextureHandle& other) const { return Handle != other.Handle; }

		static TextureHandle Null;
	private:
		UUID Handle;
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
			return hash<uint64_t>()((uint64_t)handle);
		}
	};
}