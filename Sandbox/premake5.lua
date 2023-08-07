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
		"%{wks.location}/Locus/vendor/spdlog/include",
		"%{wks.location}/Locus/src",
		"%{wks.location}/Locus/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}"
	}

	links
	{
		"Locus"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "LOCUS_DEBUG"
		runtime "Debug" -- /MDd
		symbols "on"

		linkoptions
		{
			"/ignore:4099"
		}

	filter "configurations:Release"
		defines "LOCUS_RELEASE"
		runtime "Release" -- /MD
		optimize "on"

	filter "configurations:Dist"
		defines "LOCUS_DIST"
		runtime "Release" -- /MD
		optimize "on"
		