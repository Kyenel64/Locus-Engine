#pragma once
#include "Command.h"

#include "SideA/Core/Application.h"
#include "SideA/Core/Log.h"
#include "SideA/Renderer/Texture.h"


namespace SideA
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

			Application::Get().SetIsSavedStatus(false);
		}

		virtual void Undo() override
		{
			m_TextureToChange = m_OldTexture;

			m_PathToChange = m_OldPath;

			Application::Get().SetIsSavedStatus(false);
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
