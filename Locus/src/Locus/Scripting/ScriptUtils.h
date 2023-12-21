// --- ScriptUtils ------------------------------------------------------------
// Various utility functions for the scripting engine.
#pragma once

#include <mono/metadata/assembly.h>
#include "mono/metadata/threads.h"

#include "Locus/Scripting/ScriptEngine.h"

namespace Locus
{
	namespace ScriptUtils
	{
		char* ReadBytes(const std::string& filepath, uint32_t* outSize);

		FieldType MonoTypeToFieldType(MonoType* monoType);

		bool CheckMonoError(MonoError& error);

		std::string MonoStringToUTF8(MonoString* monoString);

		// If there is an exception, read its information and push to the Exceptions queue.
		void ProcessException(MonoException* exception);
	}
}