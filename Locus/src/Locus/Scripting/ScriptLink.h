// --- ScriptLink -------------------------------------------------------------
// Scriptlink links all the C++ functions we want to call within C#.
// Also registers components so we can call component functions within C# 
// since I dont think there is a way to use templates in mono internal calls.

#pragma once

namespace Locus
{
	class ScriptLink
	{
	public:
		static void RegisterFunctions();
		static void RegisterComponents();
	};
}