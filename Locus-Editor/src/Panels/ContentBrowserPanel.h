// --- ContentBrowserPanel ----------------------------------------------------
// Content browser panel. Includes root directory view and current directory 
//	view.
#pragma once

#include <filesystem>

#include <Locus.h>

namespace Locus
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
		std::filesystem::path m_ProjectDirectory;
		std::filesystem::path m_CurrentDirectory;
		Ref<Texture2D> m_FolderIcon;
		Ref<Texture2D> m_FileIcon;
	};
}
