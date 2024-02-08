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
		void ReloadDirectory();

	private:
		void DrawTopBar();
		void DrawCurrentDirectoryView();
		void DrawProjectDirectoryView();

		void DrawItem(const std::filesystem::path& item, Ref<Texture2D> icon);

		void ProcessCreateNewMaterialPopup();

	private:
		std::filesystem::path m_AssetsDirectory;
		std::filesystem::path m_CurrentDirectory;
		std::filesystem::path m_SelectedResource;

		std::vector<std::filesystem::path> m_DirectoryItems;

		std::stack<std::string> m_Breadcrumb;

		float m_IconSize = 70.0f;

		bool m_OpenCreateNewMaterialPopup = false;
		std::string m_CreateNewMaterialName;

		// Textures
		Ref<Texture2D> m_PlusIcon;
		Ref<Texture2D> m_FolderIcon;
		Ref<Texture2D> m_FileIcon;
	};
}
