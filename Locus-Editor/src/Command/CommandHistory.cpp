#include "Lpch.h"
#include "CommandHistory.h"

#include "LocusEditorLayer.h"

namespace Locus
{
	struct CommandHistoryData
	{
		Command* Commands[1000];
		int32_t CommandSize;
		int32_t CommandPtr;
		bool FirstCommand;

		LocusEditorLayer* Editor;
	};

	static CommandHistoryData* s_Data = nullptr;

	void CommandHistory::Init(LocusEditorLayer* editor)
	{
		s_Data = new CommandHistoryData();
		s_Data->Editor = editor;
	}

	void CommandHistory::Shutdown()
	{
		for (int i = 0; i < s_Data->CommandSize; i++)
			delete s_Data->Commands[i];

		delete s_Data;
	}

	void CommandHistory::Reset()
	{
		for (int i = 0; i < s_Data->CommandSize; i++)
			delete s_Data->Commands[i];
		s_Data->CommandSize = 0;
		s_Data->CommandPtr = 0;
		s_Data->FirstCommand = true;
	}

	void CommandHistory::AddCommand(Command* cmd)
	{
		cmd->Execute();

		if (s_Data->CommandPtr < s_Data->CommandSize)
		{
			for (int32_t i = s_Data->CommandPtr + 1; i < s_Data->CommandSize - 1; i++)
				delete s_Data->Commands[i];
			s_Data->CommandSize = s_Data->CommandPtr + 1;
		}

		s_Data->Commands[s_Data->CommandSize] = cmd;
		s_Data->CommandSize++;

		if (s_Data->CommandSize > 1 && s_Data->Commands[s_Data->CommandSize - 1]->CanMerge() && s_Data->Commands[s_Data->CommandSize - 2]->CanMerge())
		{
			if (s_Data->Commands[s_Data->CommandSize - 1]->Merge(s_Data->Commands[s_Data->CommandSize - 2]))
			{
				delete s_Data->Commands[s_Data->CommandSize - 1];
				s_Data->CommandSize--;
			}
		}

		s_Data->CommandPtr = s_Data->CommandSize - 1;
		s_Data->FirstCommand = false;

		LOCUS_CORE_INFO("ADDED COMMAND. Ptr Position: {0}", s_Data->CommandPtr);
	}

	void CommandHistory::Undo()
	{
		if (s_Data->CommandPtr >= 0 && s_Data->Commands[s_Data->CommandPtr] != nullptr && !s_Data->FirstCommand)
		{
			s_Data->Commands[s_Data->CommandPtr]->Undo();
			s_Data->CommandPtr--;
			s_Data->FirstCommand = false;
			LOCUS_CORE_INFO("UNDO COMMAND. Ptr Position: {0}", s_Data->CommandPtr);
		}
	}

	void CommandHistory::Redo()
	{
		int32_t redoCommandPtr = s_Data->CommandPtr + 1;
		if (redoCommandPtr < s_Data->CommandSize && redoCommandPtr >= 0 && !s_Data->FirstCommand)
		{
			s_Data->Commands[redoCommandPtr]->Execute();
			s_Data->CommandPtr++;
			s_Data->FirstCommand = false;
			LOCUS_CORE_INFO("REDO COMMAND. Ptr Position: {0}", s_Data->CommandPtr);
		}
	}

	void CommandHistory::SetEditorSavedStatus(bool status)
	{
		s_Data->Editor->SetSavedStatus(status);
	}
}