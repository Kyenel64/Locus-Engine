#pragma once

#include "Locus/Core/UUID.h"
#include "Locus/Renderer/Material.h"

namespace Locus
{
	class MaterialHandle
	{
	public:
		MaterialHandle() : Handle(UUID()) {}
		MaterialHandle(UUID uuid) : Handle(uuid) {}
		~MaterialHandle() = default;

		operator uint64_t() const { return (uint64_t)Handle; }
		operator bool() const;
		bool operator==(const MaterialHandle& other) const { return Handle == other.Handle; }
		bool operator!=(const MaterialHandle& other) const { return Handle != other.Handle; }

		static MaterialHandle Null;
	private:
		UUID Handle;
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
			return hash<uint64_t>()((uint64_t)handle);
		}
	};
}