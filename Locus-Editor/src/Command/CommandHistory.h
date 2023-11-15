// --- CommandHistory ---------------------------------------------------------
// Manages commands in the command pattern.
#pragma once
#include "Command.h"

namespace Locus
{
	class CommandHistory
	{
	public:
		static void Init(LocusEditorLayer* editor);
		static void Shutdown();
		static void Reset();

		static void AddCommand(Command* cmd);
		static void Undo();
		static void Redo();

		static void SetEditorSavedStatus(bool status);
	};
}
