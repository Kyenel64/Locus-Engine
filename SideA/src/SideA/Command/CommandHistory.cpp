#include "SApch.h"
#include "CommandHistory.h"

namespace SideA
{
	Command* CommandHistory::m_Commands[1000] = {};
	int32_t CommandHistory::m_CommandSize = 0;
	int32_t CommandHistory::m_CommandPtr = 0;

	void CommandHistory::AddCommand(Command* cmd)
	{
		cmd->Execute();

		if (m_CommandPtr < m_CommandSize)
		{
			for (int32_t i = m_CommandPtr + 1; i < m_CommandSize - 1; i++)
				delete m_Commands[i];
			m_CommandSize = m_CommandPtr + 1;
		}

		m_Commands[m_CommandSize] = cmd;
		m_CommandSize++;

		if (m_CommandSize > 1 && m_Commands[m_CommandSize - 1]->CanMerge() && m_Commands[m_CommandSize - 2]->CanMerge())
		{
			if (m_Commands[m_CommandSize - 1]->Merge(m_Commands[m_CommandSize - 2]))
			{
				delete m_Commands[m_CommandSize - 1];
				m_CommandSize--;
			}
		}

		m_CommandPtr = m_CommandSize - 1;

		SIDEA_CORE_INFO("ADDED COMMAND. Ptr Position: {0}", m_CommandPtr);
	}

	void CommandHistory::SetNoMergeMostRecent()
	{
		if (m_CommandSize - 1 >= 0)
			m_Commands[m_CommandSize - 1]->SetNoMerge();
	}

	void CommandHistory::Undo()
	{
		if (m_CommandPtr >= 0)
		{
			SIDEA_CORE_INFO("UNDO COMMAND. Ptr Position: {0}", m_CommandPtr);
			m_Commands[m_CommandPtr]->Undo();
			m_CommandPtr--;
		}
	}

	void CommandHistory::Redo()
	{
		int32_t redoCommandPtr = m_CommandPtr + 1;
		if (redoCommandPtr < m_CommandSize && redoCommandPtr >= 0)
		{
			SIDEA_CORE_INFO("REDO COMMAND. Ptr Position: {0}", m_CommandPtr);
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