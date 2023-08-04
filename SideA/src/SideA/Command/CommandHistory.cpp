#include "SApch.h"
#include "CommandHistory.h"

namespace SideA
{
	Command* CommandHistory::m_Commands[1000] = {};
	uint32_t CommandHistory::m_CommandSize = 0;
	uint32_t CommandHistory::m_CommandPtr = 0;

	void CommandHistory::AddCommand(Command* cmd)
	{
		cmd->Execute();

		m_Commands[m_CommandPtr + 1] = cmd;
		m_CommandPtr++;
		m_CommandSize++;
		SIDEA_CORE_INFO("ADDED COMMAND. Ptr Position: {0}", m_CommandPtr);
	}

	void CommandHistory::Undo()
	{
		if (m_CommandPtr > 0)
		{
			m_Commands[m_CommandPtr]->Undo();
			m_CommandPtr--;
			SIDEA_CORE_INFO("UNDO COMMAND. Ptr Position: {0}", m_CommandPtr);
		}
	}

	void CommandHistory::Redo()
	{
		uint32_t redoCommandPtr = m_CommandPtr + 1;
		if (redoCommandPtr < m_CommandSize && redoCommandPtr >= 0)
		{
			m_Commands[redoCommandPtr]->Execute();
			m_CommandPtr++;
		}
	}

	void CommandHistory::Reset()
	{
		m_CommandSize = 0;
		m_CommandPtr = 0;
	}

}