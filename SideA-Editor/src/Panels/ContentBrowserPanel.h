#pragma once

#include <filesystem>

#include "SideA/Renderer/Texture.h"

namespace SideA
{
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		void OnImGuiRender();

	private:
		void DrawCurrentDirectoryView();
		void DrawRootDirectoryView();

	private:
		std::filesystem::path m_CurrentDirectory;
		Ref<Texture2D> m_FolderIcon;
		Ref<Texture2D> m_FileIcon;
	};
}