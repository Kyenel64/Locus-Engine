// --- ScriptEngine -----------------------------------------------------------
// Contains: ScriptEngine, ScriptClass, ScriptInstance, ScriptClassField, 
//	ScriptClassFieldInstance
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

	struct ExceptionData
	{
		std::string Type;
		std::string Message;
		std::string Trace;
	};

	// Field data. 
	// ScriptClassField is a struct for a class field. Similar to the ScriptClass, 
	//	it is not an instance and holds general information about the field.
	struct ScriptClassField
	{
		MonoClassField* MonoField;
		std::string FieldName;
		FieldType Type;
	};

	// Field instance data. 
	// ScriptClassFieldInstance is a struct for an instance of the field.
	//	Note that this struct has no relationships to a ScriptInstance. It is only
	//	used during editor-time when ScriptInstances have not been created yet. 
	//	Data is copied to the ScriptInstance fields once runtime starts.
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



	// --- ScriptEngine -------------------------------------------------------
	// ScriptEngine does all the processing and calls for our C# scripting.
	//	This is where the assembly, classes, and fields are loaded.
	class ScriptEngine
	{
	public:
		static void Init();
		static void Shutdown();

		// Sets the current scene
		static void OnRuntimeStart(Ref<Scene> scene);
		// Clears script instances
		static void OnRuntimeStop();

		static void OnCreateEntityScript(Entity entity);
		static void OnUpdateEntityScript(Entity entity, Timestep deltaTime);

		static bool HasClass(const std::string& className, const std::string& namespaceName = std::string());
		static void InvokeMethod(Ref<ScriptInstance> instance, MonoMethod* method, void** params);

		static void ReloadScripts();

		// Getters
		static MonoImage* GetImage();
		static Ref<Scene> GetScene();
		static std::vector<std::string> GetClassNames();
		static Ref<ScriptClass> GetEntityBaseClass();
		static MonoDomain* GetAppDomain();
		static Ref<ScriptInstance> GetScriptInstance(UUID id);
		static Ref<ScriptClass> GetScriptClass(const std::string& name);
		static std::map<std::string, ScriptClassFieldInstance>& GetFieldInstances(UUID id);
		static std::queue<ExceptionData>& GetExceptions();

	private:
		static void LoadAssembly(const std::string& assemblyPath, bool loadPDB = false);
		static void LoadAppAssembly(const std::string& assemblyPath, bool loadPDB = false);
		static void LoadAppAssemblyClasses();
	};



	// --- ScriptClass --------------------------------------------------------
	// ScriptClass is created for each class within the assembly.
	//	ScriptClass can create instances of the class and get
	//	information about the class.
	class ScriptClass
	{
	public:
		ScriptClass() = default;
		ScriptClass(MonoImage* image, const std::string& namespaceName, const std::string& className);
		~ScriptClass() = default;

		// Creates a mono instance of the script class.
		MonoObject* Instantiate();

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
	// ScriptInstance is an instance of a ScriptClass.
	//	An instance is created from a ScriptClass and is tied to an Entity.
	//	It holds a GCHandle which is the instance tied to the mono garbage
	//	collector. This is used to reference the instance. Not with MonoObject*.
	class ScriptInstance
	{
	public:
		ScriptInstance() = default;
		ScriptInstance(Ref<ScriptClass> scriptClass, UUID uuid);
		~ScriptInstance();

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
		UUID m_UUID;
		Ref<ScriptClass> m_ScriptClass;
		MonoMethod* m_OnCreateFunc;
		MonoMethod* m_OnUpdateFunc;
		unsigned int m_GCHandle;

	public:
		friend class ScriptEngine;
	};
}
