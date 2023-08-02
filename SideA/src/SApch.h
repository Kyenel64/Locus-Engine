// --- tpch -------------------------------------------------------------------
// Pre-compiled header for SideA.

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

#include "SideA/Core/Log.h"
#include "SideA/Debug/Instrumentor.h"

#ifdef SIDEA_PLATFORM_WINDOWS
	#include <Windows.h>
#endif