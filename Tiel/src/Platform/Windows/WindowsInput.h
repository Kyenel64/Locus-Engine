// --- WindowsInput -----------------------------------------------------------
// Input polling for windows. Provides implementations for windows specific
// inputs.

#pragma once

#include "Tiel/Core/Input.h"

namespace Tiel
{
	class WindowsInput : public Input
	{
	protected:
		virtual bool IsKeyPressedImpl(int keycode) override;

		virtual bool IsMouseButtonPressedImpl(int button) override;
		virtual std::pair<float, float> GetMousePositionImpl() override;
		virtual float GetMouseXImpl() override;
		virtual float GetMouseYImpl() override;
	};
}