#pragma once

struct GLFWwindow;

namespace LocusLauncher
{
	class Window
	{
	public:
		Window(const std::string& name, uint32_t windowWidth, uint32_t windowHeight);
		~Window();

		void OnUpdate();

		bool Init();

		GLFWwindow* GetNativeWindow() { return m_NativeWindow; }

		const std::string& GetWindowName() const { return m_WindowName; }
		uint32_t GetWidth() const { return m_WindowWidth; }
		uint32_t GetHeight() const { return m_WindowHeight; }
		void CenterGLFWWindow();

	private:
		GLFWwindow* m_NativeWindow;
		uint32_t m_WindowWidth;
		uint32_t m_WindowHeight;
		std::string m_WindowName = "Locus Launcher";
	};
}