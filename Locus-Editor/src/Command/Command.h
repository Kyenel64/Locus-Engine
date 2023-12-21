// --- Command ----------------------------------------------------------------
// Command interface for command pattern. Handles execution, undo and merging.
#pragma once

#include <Locus.h>

namespace Locus
{
	class Command
	{
	public:
		virtual ~Command() = default;

		virtual void Execute() = 0;
		virtual void Undo() = 0;
		virtual bool Merge(Command* other) = 0;

		void SetNoMerge() { m_CanMerge = false; }
		bool CanMerge() const { return m_CanMerge; }

	protected:
		bool m_CanMerge = true;
	};
}
