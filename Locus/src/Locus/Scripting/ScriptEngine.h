// --- ScriptEngine -----------------------------------------------------------
// ScriptEngine does all the processing and calls for our C# scripting.
// ScriptClass is our C# class object.
// ScriptInstance is an instance of a ScriptClass linked with an entity.

#pragma once

#include "Locus/Core/Timestep.h"
#include "Locus/Scene/Scene.h"
#include "Locus/Scene/Entity.h"

extern "C" {
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoImage MonoImage;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoDomain MonoDomain;
}


namespace Locus
{
	class Scene;
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

		static MonoImage* GetImage();
		static Scene* GetScene();
		static std::vector<std::string> GetClassNames();
		static MonoDomain* GetAppDomain();

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

		MonoObject* Instantiate();
		MonoMethod* GetMethod(const std::string& name, int paramCount);
		MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr);

	private:
		MonoImage* m_MonoImage = nullptr;
		MonoClass* m_MonoClass = nullptr;
		std::string m_NamespaceName;
		std::string m_ClassName;
	};

	
	// --- ScriptInstance -----------------------------------------------------
	class ScriptInstance
	{
	public:
		ScriptInstance() = default;
		ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity);

		void InvokeOnCreate();
		void InvokeOnUpdate(Timestep deltaTime);

	private:
		Entity m_Entity;
		Ref<ScriptClass> m_ScriptClass;
		MonoObject* m_ScriptInstance;
		MonoMethod* m_OnCreateFunc;
		MonoMethod* m_OnUpdateFunc;
	};
}