// --- ProjectBrowserPanel ----------------------------------------------------
// Project browser panel. Includes root directory view and current directory 
//	view.
#pragma once

#include <filesystem>

#include <Locus.h>

namespace Locus
{
	class ProjectBrowserPanel
	{
	public:
		ProjectBrowserPanel();

		void OnImGuiRender();

		void SetDirectory(const std::filesystem::path& path);

	private:
		void DrawTopBar();
		void DrawCurrentDirectoryView();
		void DrawProjectDirectoryView();

		void DrawItem(const std::filesystem::directory_entry& item, Ref<Texture2D> icon);

	private:
		std::filesystem::path m_AssetsDirectory;
		std::filesystem::path m_CurrentDirectory;
		std::filesystem::path m_SelectedResource;

		std::vector<std::filesystem::directory_entry> m_DirectoryItems;

		std::stack<std::string> m_Breadcrumb;

		float m_IconSize = 70.0f;

		// Textures
		Ref<Texture2D> m_PlusIcon;
		Ref<Texture2D> m_FolderIcon;
		Ref<Texture2D> m_FileIcon;
	};
}
