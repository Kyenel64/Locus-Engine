workspace ProjectName
    architecture "x86_64"
    startproject ProjectName

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

project ProjectName
	kind "SharedLib"
	language "C#"
	dotnetframework "4.7.2"
	
	targetdir ("bin")
	objdir ("bin-int")
	
	files
	{
		"Assets/**.cs"
	}

    links
    {
        LocusScriptLocation
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