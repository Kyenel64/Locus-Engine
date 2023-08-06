#include "SApch.h"
#include "ContentBrowserPanel.h"


#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>

namespace SideA
{

	// TODO: move to project folder in the future when we create project folders
	static const std::filesystem::path s_ProjectPath = "assets";

	ContentBrowserPanel::ContentBrowserPanel()
		: m_CurrentDirectory(s_ProjectPath)
	{
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");

		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowMinSize.y = 300.0f;

		if (m_CurrentDirectory != s_ProjectPath)
		{
			if (ImGui::Button("<-"))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}


		for (auto& entry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = entry.path();
			auto relativePath = std::filesystem::relative(path, s_ProjectPath);
			std::string filenameString = relativePath.filename().string();
			if (entry.is_directory())
			{
				if (ImGui::Button(filenameString.c_str()))
				{
					m_CurrentDirectory /= path.filename();
				}
			}
			else
			{
				if (ImGui::Button(filenameString.c_str()))
				{
				}
			}
		}


		ImGui::End(); // End Content Browser
	}
}