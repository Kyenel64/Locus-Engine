#include "Lpch.h"
#include "ScriptEngine.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/attrdefs.h>
#include "mono/metadata/mono-debug.h"
#include "mono/metadata/threads.h"
#include "mono/metadata/exception.h"

#include "Locus/Core/UUID.h"
#include "Locus/Scene/Components.h"
#include "Locus/Scripting/ScriptLink.h"
#include "Locus/Scripting/ScriptUtils.h"

namespace Locus
{
	// Loads a C# assembly from a path. Returns the loaded assembly.
	static MonoAssembly* LoadCSharpAssembly(const std::string& assemblyPath, bool loadPDB = false)
	{
		uint32_t fileSize = 0;
		char* fileData = ScriptUtils::ReadBytes(assemblyPath, &fileSize);

		// NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
		MonoImageOpenStatus status;
		MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

		if (status != MONO_IMAGE_OK)
		{
			const char* errorMessage = mono_image_strerror(status);
			// Log some error message using the errorMessage data
			return nullptr;
		}

		if (loadPDB)
		{
			std::filesystem::path pdbPath = assemblyPath;
			pdbPath.replace_extension(".pdb");

			if (std::filesystem::exists(pdbPath))
			{
				uint32_t pdbFileSize = 0;
				char* pdbFileData = ScriptUtils::ReadBytes(pdbPath.string(), &pdbFileSize);
				mono_debug_open_image_from_memory(image, (const mono_byte*)pdbFileData, pdbFileSize);
				LOCUS_CORE_INFO("Loaded PDB {}", pdbPath);
				delete[] pdbFileData;
			}
		}

		MonoAssembly* assembly = mono_assembly_load_from_full(image, assemblyPath.c_str(), &status, 0);
		mono_image_close(image);

		// Don't forget to free the file data
		delete[] fileData;

		return assembly;
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

		Ref<Scene> Scene = nullptr;
		
		bool Debugging = true;

		std::unordered_map<std::string, Ref<ScriptClass>> ScriptClasses;
		std::unordered_map<UUID, Ref<ScriptInstance>> ScriptInstances;
		std::vector<std::string> ScriptClassNames;
		std::map<UUID, std::map<std::string, ScriptClassFieldInstance>> FieldInstances;

		std::queue<ExceptionData> Exceptions;

		Ref<ScriptClass> EntityBaseClass;
	};

	static ScriptEngineData* s_Data = nullptr;

	typedef void (*OnUpdateThunk)(MonoObject*, float, MonoException**);
	typedef void (*OnCreateThunk)(MonoObject*, MonoException**);


	// --- ScriptEngine -------------------------------------------------------
	void ScriptEngine::Init()
	{
		s_Data = new ScriptEngineData();

		// Sets the path of .NET system libraries
		mono_set_assemblies_path("mono/lib");

		if (s_Data->Debugging)
		{
			const char* argv[1] = {
				//"--debugger-agent=transport=dt_socket,address=127.0.0.1:2550,server=y,suspend=n,loglevel=3,logfile=MonoDebugger.log",
				"--soft-breakpoints"
			};

			mono_jit_parse_options(1, (char**)argv);
			mono_debug_init(MONO_DEBUG_FORMAT_MONO);
		}

		// Initialize mono runtime
		MonoDomain* rootDomain = mono_jit_init("LocusJITRuntime");
		LOCUS_CORE_ASSERT(rootDomain, "Unable to initialize jit runtime!");
		s_Data->RootDomain = rootDomain;

		if (s_Data->Debugging)
			mono_debug_domain_create(s_Data->RootDomain);

		mono_thread_set_main(mono_thread_current());

		LoadAssembly("resources/scripts/Locus-Script.dll", s_Data->Debugging);
		LoadAppAssembly("SandboxProject/bin/Sandbox.dll", s_Data->Debugging); // TODO: set to project path
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

		LoadAssembly("resources/scripts/Locus-Script.dll", s_Data->Debugging);
		LoadAppAssembly("SandboxProject/bin/Sandbox.dll", s_Data->Debugging); // TODO: set to project path
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
	void ScriptEngine::LoadAssembly(const std::string& assemblyPath, bool loadPDB)
	{
		// Create app domain. App domains are like separate processes within mono
		s_Data->AppDomain = mono_domain_create_appdomain("LocusAppDomain", nullptr);
		LOCUS_CORE_ASSERT(s_Data->AppDomain, "Failed to create mono app domain!");
		mono_domain_set(s_Data->AppDomain, true);

		s_Data->CoreAssembly = LoadCSharpAssembly(assemblyPath, loadPDB);
		s_Data->CoreAssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);
	}

	// Loads the C# assembly that contains the user-written project code
	void ScriptEngine::LoadAppAssembly(const std::string& assemblyPath, bool loadPDB)
	{
		s_Data->AppAssembly = LoadCSharpAssembly(assemblyPath, loadPDB);
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
					classField.Type = ScriptUtils::MonoTypeToFieldType(fieldType);

					scriptClass->m_PublicFields[fieldName] = classField;
					LOCUS_CORE_INFO(" " + std::string(fieldName));
				}
			}
		}
	}

	void ScriptEngine::OnRuntimeStart(Ref<Scene> scene)
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
			s_Data->ScriptInstances[entity.GetUUID()] = CreateRef<ScriptInstance>(s_Data->ScriptClasses[sc.ScriptClass], entity.GetUUID());
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

	bool ScriptEngine::HasClass(const std::string& className, const std::string& namespaceName)
	{
		// Name formatting
		std::string classNameStr;
		if (namespaceName != std::string())
			classNameStr = std::string(namespaceName) + "::" + std::string(className);
		else
			classNameStr = std::string(className);
		return s_Data->ScriptClasses.find(classNameStr) != s_Data->ScriptClasses.end();
	}

	void ScriptEngine::InvokeMethod(Ref<ScriptInstance> instance, MonoMethod* method, void** params)
	{
		MonoObject* exception = nullptr;
		MonoObject* obj = mono_runtime_invoke(method, mono_gchandle_get_target(instance->m_GCHandle), const_cast<void**>(params), &exception);
		ScriptUtils::ProcessException((MonoException*)exception);
	}

	// Getters
	MonoImage* ScriptEngine::GetImage() { return s_Data->CoreAssemblyImage; }
	Ref<Scene> ScriptEngine::GetScene() { return s_Data->Scene; }
	std::vector<std::string> ScriptEngine::GetClassNames() { return s_Data->ScriptClassNames; }
	Ref<ScriptClass> ScriptEngine::GetEntityBaseClass() { return s_Data->EntityBaseClass; }
	MonoDomain* ScriptEngine::GetAppDomain() { return s_Data->AppDomain; }
	std::queue<ExceptionData>& ScriptEngine::GetExceptions() { return s_Data->Exceptions; }
	std::map<std::string, ScriptClassFieldInstance>& ScriptEngine::GetFieldInstances(UUID id) { return s_Data->FieldInstances[id]; }

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

	ScriptClassField& ScriptClass::GetPublicField(const std::string& name)
	{
		LOCUS_CORE_ASSERT(m_PublicFields.find(name) != m_PublicFields.end(), "Field name does not exist!");
		return m_PublicFields[name];
	}

	bool ScriptClass::GetFieldValueInternal(const std::string& name, void* buffer)
	{
		ScriptClassField& field = GetPublicField(name);
		MonoObject* tempInstance = Instantiate();
		unsigned int gchandle = mono_gchandle_new(tempInstance, false);
		mono_field_get_value(mono_gchandle_get_target(gchandle), field.MonoField, buffer);
		mono_gchandle_free(gchandle);
		return true;
	}



	// --- ScriptInstance -----------------------------------------------------
	ScriptInstance::ScriptInstance(Ref<ScriptClass> scriptClass, UUID uuid)
		: m_ScriptClass(scriptClass), m_UUID(uuid)
	{
		// Use gchandle when creating or referencing instances. 
		// MonoObject* instances are not handled and will result in errors.
		m_GCHandle = mono_gchandle_new(m_ScriptClass->Instantiate(), false);

		m_OnCreateFunc = m_ScriptClass->GetMethod("OnCreate", 0);
		m_OnUpdateFunc = m_ScriptClass->GetMethod("OnUpdate", 1);

		// Call Entity base class constructor
		MonoMethod* entityBaseConstructor = s_Data->EntityBaseClass->GetMethod(".ctor", 1);
		UUID id = m_UUID;
		void* idPtr = &id;
		MonoObject* exception = nullptr;
		mono_runtime_invoke(entityBaseConstructor, mono_gchandle_get_target(m_GCHandle), &idPtr, &exception);
		ScriptUtils::ProcessException((MonoException*)exception);
	}

	ScriptInstance::~ScriptInstance()
	{
		mono_gchandle_free(m_GCHandle);
	}

	void ScriptInstance::InvokeOnCreate()
	{
		OnCreateThunk testInteropThunk = (OnCreateThunk)mono_method_get_unmanaged_thunk(m_OnCreateFunc);
		MonoException* exception = nullptr;
		testInteropThunk(mono_gchandle_get_target(m_GCHandle), &exception);
		ScriptUtils::ProcessException(exception);
	}

	void ScriptInstance::InvokeOnUpdate(Timestep deltaTime)
	{
		// This is a more optimized way of calling functions. Need to profile to see if it actually works.
		OnUpdateThunk testInteropThunk = (OnUpdateThunk)mono_method_get_unmanaged_thunk(m_OnUpdateFunc);
		MonoException* exception = nullptr;
		testInteropThunk(mono_gchandle_get_target(m_GCHandle), deltaTime, &exception);
		ScriptUtils::ProcessException(exception);
	}

	bool ScriptInstance::GetFieldValueInternal(const std::string& name, void* buffer)
	{
		ScriptClassField& field = m_ScriptClass->GetPublicField(name);
		mono_field_get_value(mono_gchandle_get_target(m_GCHandle), field.MonoField, buffer);
		return true;
	}

	bool ScriptInstance::SetFieldValueInternal(const std::string& name, const void* value)
	{
		ScriptClassField& field = m_ScriptClass->GetPublicField(name);
		mono_field_set_value(mono_gchandle_get_target(m_GCHandle), field.MonoField, (void*)value);
		return true;
	}
}
