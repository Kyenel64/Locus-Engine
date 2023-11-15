// --- ValueCommands ----------------------------------------------------------
// Commands for datatypes.
// Contains:
//	ChangeValueCommand
//	ChangeFunctionValueCommand
//	ChangeTextureCommand
#pragma once
#include "Command.h"

#include <iostream>

class SceneCamera;

namespace Locus
{
	template<typename T>
	class ChangeValueCommand : public Command
	{
	public:
		ChangeValueCommand() = default;

		ChangeValueCommand(const T newValue, T& valueToChange)
			: m_NewValue(newValue), m_ValueToChange(valueToChange)
		{
		}

		~ChangeValueCommand() = default;

		virtual void Execute() override
		{
			m_OldValue = m_ValueToChange;
			m_ValueToChange = m_NewValue;
			CommandHistory::SetEditorSavedStatus(false);
		}

		virtual void Undo() override
		{
			m_ValueToChange = m_OldValue;
			CommandHistory::SetEditorSavedStatus(false);
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


	
	// Use for when value is only manipulatable by functions.
	template<typename T>
	class ChangeFunctionValueCommand : public Command
	{
	public:
		ChangeFunctionValueCommand() = default;

		ChangeFunctionValueCommand(std::function<void(const T&)> function, const T newValue, T& valueToChange)
			: m_NewValue(newValue), m_ValueToChange(valueToChange), m_Function(function)
		{
		}

		ChangeFunctionValueCommand(std::function<void(T)> function, const T newValue, T& valueToChange)
			: m_NewValue(newValue), m_ValueToChange(valueToChange), m_Function(function)
		{
		}

		~ChangeFunctionValueCommand() = default;

		virtual void Execute() override
		{
			m_OldValue = m_ValueToChange;
			m_ValueToChange = m_NewValue;
			m_Function(m_ValueToChange);
			CommandHistory::SetEditorSavedStatus(false);
		}

		virtual void Undo() override
		{
			m_ValueToChange = m_OldValue;
			m_Function(m_ValueToChange);
			CommandHistory::SetEditorSavedStatus(false);
		}

		virtual bool Merge(Command* other) override
		{
			ChangeFunctionValueCommand* otherCommand = dynamic_cast<ChangeFunctionValueCommand*>(other);
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
		std::function<void(const T&)> m_Function;
	};



	class ChangeTextureCommand : public Command
	{
	public:
		ChangeTextureCommand() = default;

		ChangeTextureCommand(const Ref<Texture2D> newTexture, Ref<Texture2D>& textureToChange, std::string& texturePath)
			: m_NewTexture(newTexture), m_TextureToChange(textureToChange), m_PathToChange(texturePath)
		{
			m_NewPath = newTexture->GetTexturePath();
			m_OldPath = std::string();
		}

		~ChangeTextureCommand() = default;

		virtual void Execute() override
		{
			m_OldTexture = m_TextureToChange;
			m_OldPath = m_PathToChange;

			m_TextureToChange = m_NewTexture;
			m_PathToChange = m_NewPath;
			CommandHistory::SetEditorSavedStatus(false);
		}

		virtual void Undo() override
		{
			m_TextureToChange = m_OldTexture;

			m_PathToChange = m_OldPath;
			CommandHistory::SetEditorSavedStatus(false);
		}

		virtual bool Merge(Command* other) override
		{
			return false;
		}

	private:
		Ref<Texture2D> m_NewTexture;
		Ref<Texture2D> m_OldTexture;
		Ref<Texture2D>& m_TextureToChange;

		std::string m_NewPath;
		std::string m_OldPath;
		std::string& m_PathToChange;
	};
}
