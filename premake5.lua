workspace "Tiel"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Tiel"
	location "Tiel"
	kind "SharedLib"
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
		"%{prj.name}/vendor/spdlog/include"
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
		defines "TIEL_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "TIEL_RELEASE"
		optimize "On"

	filter "configurations:Dist"
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