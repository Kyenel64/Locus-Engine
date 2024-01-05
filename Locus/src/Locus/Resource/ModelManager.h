#pragma once

#include "Locus/Core/UUID.h"
#include "Locus/Renderer/Model.h"

namespace Locus
{
	class ModelHandle
	{
	public:
		ModelHandle() : Handle(UUID()) {}
		ModelHandle(UUID uuid) : Handle(uuid) {}
		~ModelHandle() = default;

		operator uint64_t() const { return (uint64_t)Handle; }
		operator bool() const;
		bool operator==(const ModelHandle& other) const { return Handle == other.Handle; }
		bool operator!=(const ModelHandle& other) const { return Handle != other.Handle; }

		static ModelHandle Null;
	private:
		UUID Handle;
	};

	class ModelManager
	{
	public:
		ModelManager() = default;
		~ModelManager() = default;

		static void Init();

		static ModelHandle LoadModel(const std::filesystem::path& modelPath);

		static Ref<Model> GetModel(ModelHandle handle);
		static const std::unordered_map<ModelHandle, Ref<Model>> GetModels();

		static bool IsValid(ModelHandle handle);
	};
}

namespace std
{
	template<>
	struct hash<Locus::ModelHandle>
	{
		std::size_t operator()(const Locus::ModelHandle& handle) const
		{
			return hash<uint64_t>()((uint64_t)handle);
		}
	};
}