#pragma once
#include "Command.h"

namespace Locus
{
	class CommandHistory
	{
	public:
		static void AddCommand(Command* cmd);
		static void Undo();
		static void Redo();

		static void Shutdown();

		static void SetNoMergeMostRecent();

		static void Reset();

	private:
		static Command* m_Commands[1000];
		static int32_t m_CommandSize;
		static int32_t m_CommandPtr;
		static bool m_FirstCommand;
	};
}
