#include "Lpch.h"
#include "ScriptEngine.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/attrdefs.h>

#include "Locus/Core/UUID.h"
#include "Locus/Scene/Components.h"
#include "Locus/Scripting/ScriptLink.h"

namespace Locus
{
	namespace Utils
	{
		char* ReadBytes(const std::string& filepath, uint32_t* outSize)
		{
			std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

			if (!stream)
			{
				// Failed to open the file
				return nullptr;
			}

			std::streampos end = stream.tellg();
			stream.seekg(0, std::ios::beg);
			uint32_t size = (uint32_t)(end - stream.tellg());

			if (size == 0)
			{
				// File is empty
				return nullptr;
			}

			char* buffer = new char[size];
			stream.read((char*)buffer, size);
			stream.close();

			*outSize = size;
			return buffer;
		}
		// Loads a C# assembly from a path. Returns the loaded assembly.
		MonoAssembly* LoadCSharpAssembly(const std::string& assemblyPath)
		{
			uint32_t fileSize = 0;
			char* fileData = Utils::ReadBytes(assemblyPath, &fileSize);

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

		// Converts MonoType to Locus's FieldType.
		FieldType MonoTypeToFieldType(MonoType* monoType)
		{
			std::string typeName = std::string(mono_type_get_name(monoType));
			if (typeName == "System.Single")  return FieldType::SystemSingle;
			if (typeName == "System.Double")  return FieldType::SystemDouble;
			if (typeName == "System.Int16")   return FieldType::SystemShort;
			if (typeName == "System.Int32")   return FieldType::SystemInt;
			if (typeName == "System.Int64")   return FieldType::SystemLong;
			if (typeName == "System.UInt16")  return FieldType::SystemUShort;
			if (typeName == "System.UInt32")  return FieldType::SystemUInt;
			if (typeName == "System.UInt64")  return FieldType::SystemULong;
			if (typeName == "System.Boolean") return FieldType::SystemBoolean;
			if (typeName == "System.Char")    return FieldType::SystemChar;

			if (typeName == "Locus.Vec2")     return FieldType::LocusVec2;
			if (typeName == "Locus.Vec3")     return FieldType::LocusVec3;
			if (typeName == "Locus.Vec4")     return FieldType::LocusVec4;
			if (typeName == "Locus.Entity")   return FieldType::LocusEntity;
			return FieldType::None;
		}
	}

	struct ScriptEngineData
	{
		// Domains
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
		std::map<UUID, std::map<std::string, ScriptClassFieldInstance>> FieldInstances;

		Ref<ScriptClass> EntityBaseClass;
	};

	static ScriptEngineData* s_Data = nullptr;



	// --- ScriptEngine -------------------------------------------------------
	void ScriptEngine::Init()
	{
		s_Data = new ScriptEngineData();

		// Sets the path of .NET system libraries
		mono_set_assemblies_path("mono/lib");

		// Initialize mono runtime
		MonoDomain* rootDomain = mono_jit_init("LocusJITRuntime");
		LOCUS_CORE_ASSERT(rootDomain, "Unable to initialize jit runtime!");
		s_Data->RootDomain = rootDomain;

		LoadAssembly("resources/scripts/Locus-Script.dll");
		LoadAppAssembly("SandboxProject/bin/Sandbox.dll"); // TODO: set to project path
		LoadAppAssemblyClasses();

		s_Data->EntityBaseClass = CreateRef<ScriptClass>(s_Data->CoreAssemblyImage, "Locus", "Entity");

		// Links C++ internal function definitions to the respective C# method declarations.
		ScriptLink::RegisterFunctions();
	}

	// Reloads the assembly
	void ScriptEngine::ReloadScripts()
	{
		mono_domain_set(mono_get_root_domain(), false);
		mono_domain_unload(s_Data->AppDomain);

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

	// Loads the C# assembly that contains Locus's core API
	void ScriptEngine::LoadAssembly(const std::string& assemblyPath)
	{
		// Create app domain. App domains are like separate processes within mono
		s_Data->AppDomain = mono_domain_create_appdomain("LocusAppDomain", nullptr);
		LOCUS_CORE_ASSERT(s_Data->AppDomain, "Failed to create mono app domain!");
		mono_domain_set(s_Data->AppDomain, true);

		s_Data->CoreAssembly = Utils::LoadCSharpAssembly(assemblyPath);
		s_Data->CoreAssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);
	}

	// Loads the C# assembly that contains the user-written project code
	void ScriptEngine::LoadAppAssembly(const std::string& assemblyPath)
	{
		s_Data->AppAssembly = Utils::LoadCSharpAssembly(assemblyPath);
		s_Data->AppAssemblyImage = mono_assembly_get_image(s_Data->AppAssembly);
	}

	// Loads all the user-written C# classes and its public fields.
	void ScriptEngine::LoadAppAssemblyClasses()
	{
		s_Data->ScriptClasses.clear();
		s_Data->ScriptClassNames.clear();
		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(s_Data->AppAssemblyImage, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

		// For each class
		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			// Get namespace and class name
			const char* namespaceName = mono_metadata_string_heap(s_Data->AppAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* className = mono_metadata_string_heap(s_Data->AppAssemblyImage, cols[MONO_TYPEDEF_NAME]);

			// Name formatting
			std::string classNameStr;
			if (std::string(namespaceName) != std::string())
				classNameStr = std::string(namespaceName) + "::" + std::string(className);
			else
				classNameStr = std::string(className);

			// Create ScriptClass
			Ref<ScriptClass> scriptClass = CreateRef<ScriptClass>(s_Data->AppAssemblyImage, namespaceName, className);
			MonoClass* monoClass = scriptClass->GetMonoClass();
			s_Data->ScriptClasses[classNameStr] = scriptClass;
			s_Data->ScriptClassNames.push_back(classNameStr);
			LOCUS_CORE_TRACE("Loaded: {}::{}", namespaceName, className);

			// Load public fields contained in the class.
			uint32_t numOfFields = mono_class_num_fields(monoClass);
			LOCUS_CORE_INFO("Public Fields:");
			void* iterator = nullptr;
			for (uint32_t j = 0; j < numOfFields; j++)
			{
				// Get field and its attribute flag (Eg. public, private)
				MonoClassField* field = mono_class_get_fields(monoClass, &iterator);
				uint32_t flags = mono_field_get_flags(field);

				// Load field if its public
				if (flags & MONO_FIELD_ATTR_PUBLIC)
				{
					const char* fieldName = mono_field_get_name(field);
					MonoType* fieldType = mono_field_get_type(field);

					ScriptClassField classField;
					classField.FieldName = std::string(fieldName);
					classField.MonoField = field;
					classField.Type = Utils::MonoTypeToFieldType(fieldType);

					scriptClass->m_PublicFields[fieldName] = classField;
					LOCUS_CORE_INFO(" " + std::string(fieldName));
				}
			}
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
			// Creates a class instance and copies data field data from the editor to the script instance.
			s_Data->ScriptInstances[entity.GetUUID()] = CreateRef<ScriptInstance>(s_Data->ScriptClasses[sc.ScriptClass], entity);
			auto& fields = ScriptEngine::GetFieldInstances(entity.GetUUID());
			Ref<ScriptInstance> instance = ScriptEngine::GetScriptInstance(entity.GetUUID());
			for (const auto& [name, field] : fields)
			{
				instance->SetFieldValueInternal(name, field.m_Buffer);
			}
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
	MonoDomain* ScriptEngine::GetAppDomain() { return s_Data->AppDomain; }

	Ref<ScriptClass> ScriptEngine::GetScriptClass(const std::string& name)
	{ 
		if (s_Data->ScriptClasses.find(name) != s_Data->ScriptClasses.end())
			return s_Data->ScriptClasses[name];
		else
			return nullptr;
	}

	Ref<ScriptInstance> ScriptEngine::GetScriptInstance(UUID id)
	{
		if (s_Data->ScriptInstances.find(id) != s_Data->ScriptInstances.end())
			return s_Data->ScriptInstances[id];
		else
			return nullptr;
	}

	std::map<std::string, ScriptClassFieldInstance>& ScriptEngine::GetFieldInstances(UUID id)
	{
		return s_Data->FieldInstances[id];
	}



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

	ScriptClassField& ScriptClass::GetPublicField(const std::string& name)
	{
		LOCUS_CORE_ASSERT(m_PublicFields.find(name) != m_PublicFields.end(), "Field name does not exist!");
		return m_PublicFields[name];
	}

	bool ScriptClass::GetFieldValueInternal(const std::string& name, void* buffer)
	{
		ScriptClassField& field = GetPublicField(name);
		MonoObject* tempInstance = Instantiate();
		mono_field_get_value(tempInstance, field.MonoField, buffer);
		return true;
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

	bool ScriptInstance::GetFieldValueInternal(const std::string& name, void* buffer)
	{
		ScriptClassField& field = m_ScriptClass->GetPublicField(name);
		mono_field_get_value(m_ScriptInstance, field.MonoField, buffer);
		return true;
	}

	bool ScriptInstance::SetFieldValueInternal(const std::string& name, const void* value)
	{
		ScriptClassField& field = m_ScriptClass->GetPublicField(name);
		mono_field_set_value(m_ScriptInstance, field.MonoField, (void*)value);
		return true;
	}
}
