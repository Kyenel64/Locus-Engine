workspace "Tiel"
	architecture "x64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directory relative to root folder
IncludeDir = {}
IncludeDir["GLFW"] = "Tiel/vendor/GLFW/include"

include "Tiel/vendor/GLFW"

project "Tiel"
	location "Tiel"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "tpch.h"
	pchsource "Tiel/src/tpch.cpp"


	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}"
	}

	links
	{
		"GLFW",
		"opengl32.lib",
		"Dwmapi.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"TIEL_PLATFORM_WINDOWS",
			"TIEL_BUILD_DLL"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
		}

	filter "configurations:Debug"
		staticruntime "off"
		runtime "Debug"
		defines 
		{
			"TIEL_DEBUG",
			"TIEL_ENABLE_ASSERTS"
		}
		symbols "On"

	filter "configurations:Release"
		staticruntime "off"
		runtime "Release"
		defines "TIEL_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		staticruntime "off"
		runtime "Release"
		defines "TIEL_DIST"
		optimize "On"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}

	includedirs
	{
		"Tiel/vendor/spdlog/include",
		"Tiel/src"
	}

	links
	{
		"Tiel"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"TIEL_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "TIEL_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "TIEL_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "TIEL_DIST"
		optimize "On"