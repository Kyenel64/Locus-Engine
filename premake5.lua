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
IncludeDir["Glad"] = "Tiel/vendor/Glad/include"
IncludeDir["ImGui"] = "Tiel/vendor/imgui"
IncludeDir["glm"] = "Tiel/vendor/glm"

group "Dependencies"
	include "Tiel/vendor/GLFW"
	include "Tiel/vendor/Glad"
	include "Tiel/vendor/imgui"

group ""

project "Tiel"
	location "Tiel"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "tpch.h"
	pchsource "Tiel/src/tpch.cpp"


	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}"
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib",
		"Dwmapi.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"TIEL_PLATFORM_WINDOWS",
			"TIEL_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "TIEL_DEBUG"
		runtime "Debug" -- /MDd
		symbols "on"

	filter "configurations:Release"
		defines "TIEL_RELEASE"
		runtime "Release" -- /MD
		optimize "on"

	filter "configurations:Dist"
		defines "TIEL_DIST"
		runtime "Release" -- /MD
		optimize "on"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

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
		"Tiel/src",
		"Tiel/vendor",
		"%{IncludeDir.glm}"
	}

	links
	{
		"Tiel"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"TIEL_PLATFORM_WINDOWS",
		}

	filter "configurations:Debug"
		defines "TIEL_DEBUG"
		runtime "Debug" -- /MDd
		symbols "on"

	filter "configurations:Release"
		defines "TIEL_RELEASE"
		runtime "Release" -- /MD
		optimize "on"

	filter "configurations:Dist"
		defines "TIEL_DIST"
		runtime "Release" -- /MD
		optimize "on"
		