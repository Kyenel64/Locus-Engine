include "Dependencies.lua"

workspace "SideA"
	architecture "x86_64"
	startproject "SideA-Editor"

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
	include "SideA/vendor/GLFW"
	include "SideA/vendor/Glad"
	include "SideA/vendor/imgui"
	include "SideA/vendor/yaml-cpp"

group ""

include "SideA"
include "Sandbox"
include "SideA-Editor"