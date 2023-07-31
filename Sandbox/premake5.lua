project "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp",
	}

	includedirs
	{
		"%{wks.location}/SideA/vendor/spdlog/include",
		"%{wks.location}/SideA/src",
		"%{wks.location}/SideA/vendor",
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
		