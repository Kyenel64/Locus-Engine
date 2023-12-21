project "Locus-Editor"
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
		"%{wks.location}/Locus-Editor/src",
		"%{wks.location}/Locus/vendor/spdlog/include",
		"%{wks.location}/Locus/vendor",
		"%{wks.location}/Locus/src",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.ImGuizmo}"
	}
	
	links
	{
		"Locus"
	}

	postbuildcommands
	{
		"{COPY} %{prj.location}/resources %{wks.location}/bin/" .. outputdir .. "/%{prj.name}/resources",
		"{COPY} %{prj.location}/mono %{wks.location}/bin/" .. outputdir .. "/%{prj.name}/mono",
		"{COPY} %{prj.location}/SandboxProject %{wks.location}/bin/" .. outputdir .. "/%{prj.name}/SandboxProject",
		"{COPY} %{prj.location}/imgui.ini %{wks.location}/bin/" .. outputdir .. "/%{prj.name}/"
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

		postbuildcommands
		{
			"{COPY} %{VULKAN_SDK}/Bin/shaderc_sharedd.dll %{wks.location}/bin/" .. outputdir .. "/%{prj.name}/"
		}

	filter "configurations:Release"
		defines "LOCUS_RELEASE"
		runtime "Release" -- /MD
		optimize "on"

		postbuildcommands
		{
			"{COPY} %{VULKAN_SDK}/Bin/shaderc_shared.dll %{wks.location}/bin/" .. outputdir .. "/%{prj.name}/"
		}

	filter "configurations:Dist"
		defines "LOCUS_DIST"
		runtime "Release" -- /MD
		optimize "on"

		postbuildcommands
		{
			"{COPY} %{VULKAN_SDK}/Bin/shaderc_shared.dll %{wks.location}/bin/" .. outputdir .. "/%{prj.name}/"
		}

			