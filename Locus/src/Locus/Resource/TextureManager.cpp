#include "Lpch.h"
#include "TextureManager.h"

#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>

#include "Locus/Resource/ResourceManager.h"
#include "Locus/Core/Application.h"

namespace Locus
{
	TextureHandle TextureHandle::Null = TextureHandle();

	struct TextureManagerData
	{
		std::unordered_map<TextureHandle, Ref<Texture2D>> Textures;
		uint32_t TextureCount = 0;
	};

	static TextureManagerData s_TMData;

	void TextureManager::Init()
	{
		LOCUS_CORE_INFO("Texture Manager:");
		// For each texture file in the assets directory
		// LoadTexture. 
		for (auto& texturePath : ResourceManager::GetTexturePaths())
		{
			LoadTexture(texturePath);
		}
	}

	TextureHandle TextureManager::LoadTexture(const std::filesystem::path& texturePath)
	{
		std::filesystem::path metaPath = texturePath;
		std::filesystem::path metadataPath = Application::Get().GetProjectPath() / metaPath.replace_extension(".meta");
		if (std::filesystem::exists(metadataPath))
		{
			// Load texture from metadata path. Temp. Load metadata when implemented
			YAML::Node data = YAML::LoadFile(metadataPath.string());
			TextureHandle texHandle = TextureHandle(texturePath);
			s_TMData.Textures[texHandle] = Texture2D::Create(Application::Get().GetProjectPath() / texturePath.string());
			s_TMData.TextureCount++;
			LOCUS_CORE_TRACE("  Loaded texture: {0}", texturePath);
			return texHandle;
		}
		else
		{
			TextureHandle texHandle = TextureHandle(texturePath);
			s_TMData.Textures[texHandle] = Texture2D::Create(Application::Get().GetProjectPath() / texturePath.string());
			s_TMData.TextureCount++;
			YAML::Emitter out;
			out << YAML::BeginMap; // Scene
			out << YAML::Key << "Path" << YAML::Value << Application::Get().GetProjectPath().string() + "/" + texturePath.string();
			out << YAML::EndMap; // End Scene
			std::ofstream fout(metadataPath);
			fout << out.c_str();
			LOCUS_CORE_TRACE("  Generated metadata: {0}", metadataPath);
			LOCUS_CORE_TRACE("  Loaded texture: {0}", texturePath);
			return texHandle;
		}
		
		return {};
	}

	Ref<Texture2D> TextureManager::GetTexture(TextureHandle handle)
	{
		if (s_TMData.Textures.find(handle) != s_TMData.Textures.end())
			return s_TMData.Textures[handle];
		return nullptr;
	}

	const std::unordered_map<TextureHandle, Ref<Texture2D>> TextureManager::GetTextures() { return s_TMData.Textures; }

	bool TextureManager::IsValid(TextureHandle handle) 
	{ 
		return s_TMData.Textures.find(handle) != s_TMData.Textures.end(); 
	}

	Ref<Texture2D> TextureHandle::Get() const
	{
		return TextureManager::GetTexture(m_Path);
	}

	TextureHandle::operator bool() const
	{
		return TextureManager::IsValid(m_Path);
	}

}