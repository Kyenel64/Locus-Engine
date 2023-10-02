#include "Lpch.h"
#include "ScriptEngine.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

#include "Locus/Core/UUID.h"
#include "Locus/Scene/Components.h"
#include "Locus/Scripting/ScriptLink.h"
#include "Locus/Utils/FileSystem.h"

namespace Locus
{
	namespace Utils
	{
		MonoAssembly* LoadCSharpAssembly(const std::string& assemblyPath)
		{
			uint32_t fileSize = 0;
			char* fileData = FileSystem::ReadBytes(assemblyPath, &fileSize);

			// NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
			MonoImageOpenStatus status;
			MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

			if (status != MONO_IMAGE_OK)
			{
				const char* errorMessage = mono_image_strerror(status);
				// Log some error message using the errorMessage data
				return nullptr;
			}

			MonoAssembly* assembly = mono_assembly_load_from_full(image, assemblyPath.c_str(), &status, 0);
			mono_image_close(image);

			// Don't forget to free the file data
			delete[] fileData;

			return assembly;
		}
	}

	struct ScriptEngineData
	{
		MonoDomain* RootDomain = nullptr;
		MonoDomain* AppDomain = nullptr;

		// Core C# assembly
		MonoAssembly* CoreAssembly = nullptr;
		MonoImage* CoreAssemblyImage = nullptr;
		// Application C# assembly
		MonoAssembly* AppAssembly = nullptr;
		MonoImage* AppAssemblyImage = nullptr;

		Scene* Scene = nullptr;

		std::unordered_map<std::string, Ref<ScriptClass>> ScriptClasses;
		std::unordered_map<UUID, Ref<ScriptInstance>> ScriptInstances;
		std::vector<std::string> ScriptClassNames;

		std::filesystem::path ProjectPath;

		Ref<ScriptClass> EntityBaseClass;
	};

	static ScriptEngineData* s_Data = nullptr;

	void ScriptEngine::Init()
	{
		s_Data = new ScriptEngineData();

		// Sets path of .NET libraries
		mono_set_assemblies_path("mono/lib");

		// Initialize mono runtime
		MonoDomain* rootDomain = mono_jit_init("LocusJITRuntime");
		LOCUS_CORE_ASSERT(rootDomain, "Unable to initialize jit runtime!");
		s_Data->RootDomain = rootDomain;

		LoadAssembly("resources/scripts/Locus-Script.dll");
		LoadAppAssembly("SandboxProject/bin/Sandbox.dll"); // TODO: set to project path
		LoadAppAssemblyClasses();

		s_Data->EntityBaseClass = CreateRef<ScriptClass>(s_Data->CoreAssemblyImage, "Locus", "Entity");

		// Link C++ functions to C# declarations
		ScriptLink::RegisterFunctions();
	}

	void ScriptEngine::Shutdown()
	{
		s_Data->RootDomain = nullptr;
		s_Data->AppDomain = nullptr;
		delete s_Data;
	}

	void ScriptEngine::LoadAssembly(const std::string& assemblyPath)
	{
		// Create app domain. App domains are like separate processes within mono
		s_Data->AppDomain = mono_domain_create_appdomain("LocusAppDomain", nullptr);
		LOCUS_CORE_ASSERT(s_Data->AppDomain, "Failed to create mono app domain!");
		mono_domain_set(s_Data->AppDomain, true);

		s_Data->CoreAssembly = Utils::LoadCSharpAssembly(assemblyPath);
		s_Data->CoreAssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);
	}

	void ScriptEngine::LoadAppAssembly(const std::string& assemblyPath)
	{
		s_Data->AppAssembly = Utils::LoadCSharpAssembly(assemblyPath);
		s_Data->AppAssemblyImage = mono_assembly_get_image(s_Data->AppAssembly);
	}

	void ScriptEngine::LoadAppAssemblyClasses()
	{
		s_Data->ScriptClasses.clear();
		s_Data->ScriptClassNames.clear();
		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(s_Data->AppAssemblyImage, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* namespaceName = mono_metadata_string_heap(s_Data->AppAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* className = mono_metadata_string_heap(s_Data->AppAssemblyImage, cols[MONO_TYPEDEF_NAME]);

			std::string classNameStr;
			if (std::string(namespaceName) != std::string())
				classNameStr = std::string(namespaceName) + "::" + std::string(className);
			else
				classNameStr = std::string(className);
			s_Data->ScriptClasses[classNameStr] = CreateRef<ScriptClass>(s_Data->AppAssemblyImage, namespaceName, className);
			s_Data->ScriptClassNames.push_back(classNameStr);
			LOCUS_CORE_TRACE("Loaded: {}::{}", namespaceName, className);

		}
	}

	void ScriptEngine::OnRuntimeStart(Scene* scene)
	{
		s_Data->Scene = scene; 
	}

	void ScriptEngine::OnRuntimeStop()
	{ 
		s_Data->ScriptInstances.clear();
		s_Data->Scene = nullptr; 
	}

	// Creates an instance of the script class and calls Entity
	void ScriptEngine::OnCreateEntityScript(Entity entity)
	{
		auto& sc = entity.GetComponent<ScriptComponent>();
		if (s_Data->ScriptClasses.find(sc.ScriptClass) != s_Data->ScriptClasses.end())
		{
			s_Data->ScriptInstances[entity.GetUUID()] = CreateRef<ScriptInstance>(s_Data->ScriptClasses[sc.ScriptClass], entity);
			s_Data->ScriptInstances[entity.GetUUID()]->InvokeOnCreate();
		}
		else
		{
			LOCUS_CORE_ERROR("Unable to find class!");
		}
	}

	void ScriptEngine::OnUpdateEntityScript(Entity entity, Timestep deltaTime)
	{
		if (s_Data->ScriptInstances.find(entity.GetUUID()) != s_Data->ScriptInstances.end())
		{
			s_Data->ScriptInstances[entity.GetUUID()]->InvokeOnUpdate(deltaTime);
		}
		else
		{
			LOCUS_CORE_ERROR("Unable to find class!");
		}
	}

	MonoImage* ScriptEngine::GetImage() { return s_Data->CoreAssemblyImage; }
	Scene* ScriptEngine::GetScene() { return s_Data->Scene; }
	std::vector<std::string> ScriptEngine::GetClassNames() { return s_Data->ScriptClassNames; }

	void ScriptEngine::SetProjectPath(const std::filesystem::path& path) { s_Data->ProjectPath = path; }


	// --- ScriptClass --------------------------------------------------------
	ScriptClass::ScriptClass(MonoImage* image, const std::string& namespaceName, const std::string& className)
		: m_NamespaceName(namespaceName), m_ClassName(className), m_MonoImage(image)
	{
		// Get class from C# assembly. Case sensitive
		m_MonoClass = mono_class_from_name_case(m_MonoImage, m_NamespaceName.c_str(), m_ClassName.c_str());
		LOCUS_CORE_ASSERT(m_MonoClass, "Could not create mono class!");
	}

	MonoObject* ScriptClass::Instantiate()
	{
		// Create mono object and call constructor
		MonoObject* instance = mono_object_new(s_Data->AppDomain, m_MonoClass);
		mono_runtime_object_init(instance);
		return instance;
	}

	MonoMethod* ScriptClass::GetMethod(const std::string& name, int paramCount)
	{
		return mono_class_get_method_from_name(m_MonoClass, name.c_str(), paramCount);
	}

	MonoObject* ScriptClass::InvokeMethod(MonoObject* instance, MonoMethod* method, void** params)
	{
		return mono_runtime_invoke(method, instance, params, nullptr);
	}


	// --- ScriptInstance -----------------------------------------------------
	ScriptInstance::ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity)
		: m_ScriptClass(scriptClass), m_Entity(entity)
	{
		UUID id = m_Entity.GetComponent<IDComponent>().ID;
		// Create instance of class linked to entity.
		m_ScriptInstance = m_ScriptClass->Instantiate();

		m_OnCreateFunc = m_ScriptClass->GetMethod("OnCreate", 0);
		m_OnUpdateFunc = m_ScriptClass->GetMethod("OnUpdate", 1);

		// Call Entity base class constructor
		MonoMethod* entityBaseConstructor = s_Data->EntityBaseClass->GetMethod(".ctor", 1);
		void* idPtr = &id;
		// Make sure class derives from Entity and functions are correct including params
		m_ScriptClass->InvokeMethod(m_ScriptInstance, entityBaseConstructor, &idPtr);

	}

	void ScriptInstance::InvokeOnCreate()
	{
		m_ScriptClass->InvokeMethod(m_ScriptInstance, m_OnCreateFunc);
	}

	void ScriptInstance::InvokeOnUpdate(Timestep deltaTime)
	{
		void* dTParam = &deltaTime;
		m_ScriptClass->InvokeMethod(m_ScriptInstance, m_OnUpdateFunc, &dTParam);
	}
}
