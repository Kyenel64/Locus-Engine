#include "LLpch.h"
#include "FileDialog.h"

#include <ShlObj_core.h>

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "Core/Launcher.h"

namespace LocusLauncher
{
	namespace Utils
	{
		std::string SelectPath()
		{
			IFileDialog* pfd;
			LPWSTR path = {};
			if (SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd))))
			{
				DWORD dwOptions;
				if (SUCCEEDED(pfd->GetOptions(&dwOptions)))
				{
					pfd->SetOptions(dwOptions | FOS_PICKFOLDERS);
				}
				if (SUCCEEDED(pfd->Show(NULL)))
				{
					IShellItem* psi;
					if (SUCCEEDED(pfd->GetResult(&psi)))
					{
						if (!SUCCEEDED(psi->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &path)))
						{
							MessageBox(NULL, _T("GetIDListName() failed"), NULL, NULL);
						}
						psi->Release();
					}
				}
				pfd->Release();
			}
			if (path)
			{
				std::wstring wStr = std::wstring(path);
				return std::string(wStr.begin(), wStr.end());
			}

			return std::string();
		}

		std::string OpenFile(const char* filter)
		{
			OPENFILENAMEA ofn;
			CHAR szFile[260] = { 0 };
			ZeroMemory(&ofn, sizeof(OPENFILENAME));
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = glfwGetWin32Window(Launcher::Get().GetWindow().GetNativeWindow());
			ofn.lpstrFile = szFile;
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = filter;
			ofn.nFilterIndex = 1;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
			if (GetOpenFileNameA(&ofn) == TRUE)
			{
				return ofn.lpstrFile;
			}
			return std::string();
		}
	}
}