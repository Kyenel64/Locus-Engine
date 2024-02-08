#pragma once

namespace LocusLauncher
{
	class GUI
	{
	public:
		GUI();
		~GUI();

		bool Init();

		void Begin();
		void End();

	private:
		void SetDarkTheme();
	};
}