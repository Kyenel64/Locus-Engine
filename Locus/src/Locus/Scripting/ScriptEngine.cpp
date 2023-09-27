#include "Lpch.h"
#include "ScriptEngine.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

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

		void PrintAssemblyTypes(MonoAssembly* assembly)
		{
			MonoImage* image = mono_assembly_get_image(assembly);
			const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
			int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

			for (int32_t i = 0; i < numTypes; i++)
			{
				uint32_t cols[MONO_TYPEDEF_SIZE];
				mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

				const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
				const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

				LOCUS_CORE_TRACE("{}.{}", nameSpace, name);
			}
		}
	}

	struct ScriptEngineData
	{
		MonoDomain* RootDomain = nullptr;
		MonoDomain* AppDomain = nullptr;

		MonoAssembly* CoreAssembly = nullptr;
		MonoImage* CoreAssemblyImage = nullptr;
	};

	extern ScriptEngineData* s_Data = nullptr;

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

		// Link C++ functions to C# declarations
		//ScriptLink::RegisterFunctions();

		// --- Example API ---
		ScriptClass entityClass = ScriptClass("Locus", "Entity");
		MonoObject* entityInstance = entityClass.Instantiate();
		// Example function
		MonoMethod* printIDFunc = entityClass.GetMethod("PrintID", 0);
		entityClass.InvokeMethod(entityInstance, printIDFunc);
		// Example function with string params
		MonoString* monoStr = mono_string_new(s_Data->AppDomain, "Hello from C++");
		void* strParam = monoStr;
		MonoMethod* printAStringFunc = entityClass.GetMethod("PrintAString", 1);
		entityClass.InvokeMethod(entityInstance, printAStringFunc, &strParam);

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
		mono_domain_set(s_Data->AppDomain, true);

		s_Data->CoreAssembly = Utils::LoadCSharpAssembly("resources/scripts/Locus-Script.dll");
		s_Data->CoreAssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);
	}



	// --- ScriptClass --------------------------------------------------------
	ScriptClass::ScriptClass(const std::string& namespaceName, const std::string& className)
		: m_NamespaceName(namespaceName), m_ClassName(className)
	{
		// Get class from C# assembly
		m_MonoClass = mono_class_from_name(s_Data->CoreAssemblyImage, m_NamespaceName.c_str(), m_ClassName.c_str());
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
}
