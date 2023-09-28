#include "Lpch.h"
#include "ScriptLink.h"

#include <mono/jit/jit.h>

namespace Locus
{

	static void Log(float val)
	{
		std::cout << val << std::endl;
	}

	void ScriptLink::RegisterFunctions()
	{
		mono_add_internal_call("Locus.InternalCalls::Log", Log);
	}
}