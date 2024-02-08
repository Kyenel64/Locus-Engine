#pragma once

#include "Locus/Core/UUID.h"
#include "Locus/Renderer/Material.h"

namespace Locus
{
	class MaterialHandle
	{
	public:
		MaterialHandle() : m_Path(std::filesystem::path()) {}
		MaterialHandle(const std::filesystem::path& path) : m_Path(path) {}
		~MaterialHandle() = default;

		Ref<Material> Get() const;

		operator std::string() const { return m_Path.string(); }
		operator bool() const;
		bool operator==(const MaterialHandle& other) const { return m_Path == other.m_Path; }
		bool operator!=(const MaterialHandle& other) const { return m_Path != other.m_Path; }

		static MaterialHandle Null;
	private:
		std::filesystem::path m_Path;
	};

	class MaterialManager
	{
	public:
		MaterialManager() = default;
		~MaterialManager() = default;

		static void Init();

		static MaterialHandle LoadMaterial(const std::filesystem::path& materialPath);

		static Ref<Material> GetMaterial(MaterialHandle handle);
		static const std::unordered_map<MaterialHandle, Ref<Material>> GetMaterials();

		static bool IsValid(MaterialHandle handle);
	};
}

namespace std
{
	template<>
	struct hash<Locus::MaterialHandle>
	{
		std::size_t operator()(const Locus::MaterialHandle& handle) const
		{
			return hash<std::string>()(handle);
		}
	};
}