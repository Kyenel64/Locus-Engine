// --- tpch -------------------------------------------------------------------
// Pre-compiled header for Locus.
#pragma once

#include <iostream>
#include <stdio.h>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <filesystem>

#include <string>
#include <sstream>
#include <vector>
#include <array>
#include <unordered_map>
#include <unordered_set>

#include "Locus/Core/Core.h"
#include "Locus/Core/Log.h"
#include "Locus/Debug/Instrumentor.h"

#ifdef LOCUS_PLATFORM_WINDOWS
	#include <Windows.h>
#endif
