project "Locus-Script"
	kind "SharedLib"
	language "C#"
	dotnetframework "4.7.2"
	
	targetdir ("../Locus-Editor/resources/scripts")
	objdir ("../Locus-Editor/resources/scripts/int")

	files
	{
		"src/**.cs"
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
				