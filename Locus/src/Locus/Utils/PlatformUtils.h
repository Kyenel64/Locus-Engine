// --- PlatformUtils ----------------------------------------------------------
// Utils for platform dependent apis.
// Implementation for this will be handled in:
//    Platform/{Platform}/{Platform}PlatformUtils.cpp
#pragma once

#include <string>

namespace Locus
{
	class FileDialogs
	{
	public:
		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);
	};
}