#pragma once

#include "Locus/Core/UUID.h"
#include "Locus/Renderer/Model.h"

namespace Locus
{
	class ModelHandle
	{
	public:
		ModelHandle() : m_Path(std::filesystem::path()) {}
		ModelHandle(const std::filesystem::path& path) : m_Path(path) {}
		~ModelHandle() = default;

		Ref<Model> Get() const;

		operator std::string() const { return m_Path.string(); }
		operator bool() const;
		bool operator==(const ModelHandle& other) const { return m_Path == other.m_Path; }
		bool operator!=(const ModelHandle& other) const { return m_Path != other.m_Path; }

		static ModelHandle Null;
	private:
		std::filesystem::path m_Path;
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
			return hash<std::string>()(handle);
		}
	};
}