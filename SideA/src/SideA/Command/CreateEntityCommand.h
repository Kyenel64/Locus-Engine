#pragma once
#include "Command.h"

namespace SideA
{
	class CreateEntityCommand : public Command
	{
	public:
		CreateEntityCommand() = default;

		CreateEntityCommand()
		{

		}

		virtual void Execute() override
		{
		}

		virtual void Undo() override
		{
		}

	private:

	};
}