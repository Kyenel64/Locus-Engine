workspace "Sandbox"
    architecture "x86_64"
    startproject "Sandbox"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

    flags
    {
        "MultiProcessorCompile"
    }

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Sandbox"
	kind "SharedLib"
	language "C#"
	dotnetframework "4.7.2"
	
	targetdir ("bin")
	objdir ("int")
	
	files
	{
		"Assets/**.cs"
	}

    links
    {
        "Locus-Script"
    }

	filter "configurations:Debug"
		optimize "off"
		symbols "Default"

	filter "configurations:Release"
		optimize "on"
		symbols "Default"
	
	filter "configurations:Dist"
		optimize "full"
		symbols "off"

group "Locus"
    include ("C:/Users/Kye/Desktop/Locus-Engine/Locus-Script")
group ""