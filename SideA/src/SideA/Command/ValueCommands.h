#pragma once
#include "Command.h"

#include "SideA/Core/Application.h"
#include "SideA/Core/Log.h"


namespace SideA
{
	template<typename T>
	class ChangeValueCommand : public Command
	{
	public:
		ChangeValueCommand() = default;

		ChangeValueCommand(const T& newValue, T& valueToChange)
			: m_NewValue(newValue), m_ValueToChange(valueToChange)
		{
		}

		virtual void Execute() override
		{
			m_OldValue = m_ValueToChange;
			m_ValueToChange = m_NewValue;
			Application::Get().SetIsSavedStatus(false);
		}

		virtual void Undo() override
		{
			m_ValueToChange = m_OldValue;
			Application::Get().SetIsSavedStatus(false);
		}

		virtual bool Merge(Command* other) override
		{
			ChangeValueCommand* otherCommand = dynamic_cast<ChangeValueCommand*>(other);
			if (otherCommand != nullptr)
			{
				if (&otherCommand->m_ValueToChange == &this->m_ValueToChange)
				{
					otherCommand->m_NewValue = this->m_NewValue;
					return true;
				}
			}
				
			return false;
		}

	private:
		T m_NewValue;
		T m_OldValue;
		T& m_ValueToChange;
	};
}
