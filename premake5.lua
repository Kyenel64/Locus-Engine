include "Dependencies.lua"

workspace "Locus"
	architecture "x86_64"
	startproject "Locus-Editor"

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

group "Dependencies"
	include "Locus/vendor/GLFW"
	include "Locus/vendor/Glad"
	include "Locus/vendor/imgui"
	include "Locus/vendor/yaml-cpp"
	include "Locus/vendor/Box2D"

group ""

include "Locus"
include "Locus-Editor"
include "Locus-Script"