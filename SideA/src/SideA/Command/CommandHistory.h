#pragma once
#include "Command.h"

#include "SideA/Core/Core.h"

namespace SideA
{
	class CommandHistory
	{
	public:
		static void AddCommand(Command* cmd);
		static void Undo();
		static void Redo();

		static void Reset();

	private:
		static Command* m_Commands[1000];
		static uint32_t m_CommandSize;
		static uint32_t m_CommandPtr;
	};
}
