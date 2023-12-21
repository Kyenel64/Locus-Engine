// --- PlatformUtils ----------------------------------------------------------
// Utils for platform dependent apis. Implementation is in platform specific
//	directory.
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
