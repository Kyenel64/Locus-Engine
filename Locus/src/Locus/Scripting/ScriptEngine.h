// --- ScriptEngine -----------------------------------------------------------
// Contains: ScriptEngine, ScriptClass, ScriptInstance, ScriptClassField, 
//	ScriptClassFieldInstance
// 
// ScriptEngine does all the processing and calls for our C# scripting.
//	This is where the assembly, classes, and fields are loaded.
// 
// ScriptClass is created for each class within the assembly.
//	This is not an instance of the class, so only one ScriptClass is created
//	for each C# class.
// 
// ScriptInstance is an instance of a ScriptClass.
//	An instance is created from a ScriptClass and is tied to an Entity.
//	ScriptInstance calls methods and accesses/modifies fields.
//
// ScriptClassField is a struct for a class field. Similar to the ScriptClass, 
//	it is not an instance and holds general information about the field.
//
// ScriptClassFieldInstance is a struct for an instance of the field.
//	Note that this struct has no relationships to a ScriptInstance. It is only
//	used during editor-time when ScriptInstances have not been created yet. 
//	Data is copied to the ScriptInstance fields once runtime starts.
#pragma once

#include "Locus/Core/Timestep.h"
#include "Locus/Scene/Scene.h"
#include "Locus/Scene/Entity.h"

// C declarations
extern "C" {
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoImage MonoImage;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoDomain MonoDomain;
	typedef struct _MonoClassField MonoClassField;
}


namespace Locus
{
	// Forward declarations
	class ScriptClass;
	class ScriptInstance;
	struct ScriptClassField;
	struct ScriptClassFieldInstance;

	enum class FieldType
	{
		None = 0,
		SystemSingle, // float
		SystemDouble,
		SystemShort,
		SystemInt,
		SystemLong,
		SystemUShort,
		SystemUInt,
		SystemULong,
		SystemBoolean,
		SystemChar,
		SystemString,

		LocusVec2,
		LocusVec3,
		LocusVec4,
		LocusEntity
	};



	// --- ScriptEngine -------------------------------------------------------
	class ScriptEngine
	{
	public:
		static void Init();
		static void Shutdown();

		static void OnRuntimeStart(Scene* scene);
		static void OnRuntimeStop();

		static void OnCreateEntityScript(Entity entity);
		static void OnUpdateEntityScript(Entity entity, Timestep deltaTime);

		static void ReloadScripts();

		// Getters
		static MonoImage* GetImage();
		static Scene* GetScene();
		static std::vector<std::string> GetClassNames();
		static MonoDomain* GetAppDomain();
		static Ref<ScriptInstance> GetScriptInstance(UUID id);
		static Ref<ScriptClass> GetScriptClass(const std::string& name);
		static std::map<std::string, ScriptClassFieldInstance>& GetFieldInstances(UUID id);

	private:
		static void LoadAssembly(const std::string& assemblyPath);
		static void LoadAppAssembly(const std::string& assemblyPath);
		static void LoadAppAssemblyClasses();
	};



	// --- ScriptClass --------------------------------------------------------
	class ScriptClass
	{
	public:
		ScriptClass() = default;
		ScriptClass(MonoImage* image, const std::string& namespaceName, const std::string& className);
		~ScriptClass() = default;

		// Creates a mono instance of the script class.
		MonoObject* Instantiate();
		// Calls a method contained in the class.
		MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr);

		// Getters
		MonoClass* GetMonoClass() const { return m_MonoClass; }
		MonoMethod* GetMethod(const std::string& name, int paramCount);
		const std::map<std::string, ScriptClassField>& GetPublicFields() const { return m_PublicFields; }
		ScriptClassField& GetPublicField(const std::string& name);

		// Gets the default value of a C# field.
		template<typename T>
		T GetFieldValue(const std::string& name)
		{
			char fieldValueBuffer[16];
			bool success = GetFieldValueInternal(name, fieldValueBuffer);
			if (!success)
				return T();
			return *(T*)fieldValueBuffer;
		}

	private:
		bool GetFieldValueInternal(const std::string& name, void* fieldValueBuffer);

	private:
		MonoImage* m_MonoImage = nullptr;
		MonoClass* m_MonoClass = nullptr;
		std::string m_NamespaceName;
		std::string m_ClassName;

		std::map<std::string, ScriptClassField> m_PublicFields;

	public:
		friend class ScriptEngine;
	};


	
	// --- ScriptInstance -----------------------------------------------------
	class ScriptInstance
	{
	public:
		ScriptInstance() = default;
		ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity);
		~ScriptInstance() = default;

		void InvokeOnCreate();
		void InvokeOnUpdate(Timestep deltaTime);

		// Gets the current value of a C# field
		template<typename T>
		T GetFieldValue(const std::string& name)
		{
			char fieldValueBuffer[16];
			bool success = GetFieldValueInternal(name, fieldValueBuffer);
			if (!success)
				return T();
			return *(T*)fieldValueBuffer;
		}

		template<typename T>
		void SetFieldValue(const std::string& name, const T& value)
		{
			SetFieldValueInternal(name, &value);
		}

	private:
		bool GetFieldValueInternal(const std::string& name, void* buffer);
		bool SetFieldValueInternal(const std::string& name, const void* value);

	private:
		Entity m_Entity;
		Ref<ScriptClass> m_ScriptClass;
		MonoObject* m_ScriptInstance;
		MonoMethod* m_OnCreateFunc;
		MonoMethod* m_OnUpdateFunc;

	public:
		friend class ScriptEngine;
	};



	// Field data. 
	struct ScriptClassField
	{
		MonoClassField* MonoField;
		std::string FieldName;
		FieldType Type;
	};

	// Field instance data. 
	struct ScriptClassFieldInstance
	{
		ScriptClassField Field;

		ScriptClassFieldInstance()
		{
			memset(m_Buffer, 0, sizeof(m_Buffer));
		}

		template<typename T>
		T GetValue()
		{
			static_assert(sizeof(T) <= 16, "Type too large!");
			return *(T*)m_Buffer;
		}
		
		template<typename T>
		void SetValue(T value)
		{
			static_assert(sizeof(T) <= 16, "Type too large!");
			memcpy(m_Buffer, &value, sizeof(T));
		}

	private:
		uint8_t m_Buffer[16];

		friend class ScriptEngine;
		friend class ScriptInstance;
		friend class PropertiesPanel;
	};
}
