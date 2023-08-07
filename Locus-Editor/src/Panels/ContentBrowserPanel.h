#pragma once

#include <filesystem>

#include "Locus/Renderer/Texture.h"

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
		std::filesystem::path m_CurrentDirectory;
		Ref<Texture2D> m_FolderIcon;
		Ref<Texture2D> m_FileIcon;
	};
}