workspace "SideA"
	architecture "x86_64"
	startproject "SideA-Editor"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directory relative to root folder
IncludeDir = {}
IncludeDir["GLFW"] = "SideA/vendor/GLFW/include"
IncludeDir["Glad"] = "SideA/vendor/Glad/include"
IncludeDir["ImGui"] = "SideA/vendor/imgui"
IncludeDir["glm"] = "SideA/vendor/glm"
IncludeDir["stb_image"] = "SideA/vendor/stb_image"
IncludeDir["entt"] = "SideA/vendor/entt/include"

group "Dependencies"
	include "SideA/vendor/GLFW"
	include "SideA/vendor/Glad"
	include "SideA/vendor/imgui"

group ""

project "SideA"
	location "SideA"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "SApch.h"
	pchsource "SideA/src/SApch.cpp"


	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp",
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
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.entt}"
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"	
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "SIDEA_DEBUG"
		runtime "Debug" -- /MDd
		symbols "on"

	filter "configurations:Release"
		defines "SIDEA_RELEASE"
		runtime "Release" -- /MD
		optimize "on"

	filter "configurations:Dist"
		defines "SIDEA_DIST"
		runtime "Release" -- /MD
		optimize "on"

project "SideA-Editor"
	location "SideA-Editor"
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
		"SideA/vendor/spdlog/include",
		"SideA/src",
		"SideA/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}"
	}
	
	links
	{
		"SideA"
	}
	
	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "SIDEA_DEBUG"
		runtime "Debug" -- /MDd
		symbols "on"

	filter "configurations:Release"
		defines "SIDEA_RELEASE"
		runtime "Release" -- /MD
		optimize "on"
	
	filter "configurations:Dist"
		defines "SIDEA_DIST"
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
		"SideA/vendor/spdlog/include",
		"SideA/src",
		"SideA/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}"
	}

	links
	{
		"SideA"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "SIDEA_DEBUG"
		runtime "Debug" -- /MDd
		symbols "on"

	filter "configurations:Release"
		defines "SIDEA_RELEASE"
		runtime "Release" -- /MD
		optimize "on"

	filter "configurations:Dist"
		defines "SIDEA_DIST"
		runtime "Release" -- /MD
		optimize "on"
		