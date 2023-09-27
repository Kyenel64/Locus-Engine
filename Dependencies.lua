-- Locus Dependencies

VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/Locus/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/Locus/vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/Locus/vendor/imgui"
IncludeDir["glm"] = "%{wks.location}/Locus/vendor/glm"
IncludeDir["stb_image"] = "%{wks.location}/Locus/vendor/stb_image"
IncludeDir["entt"] = "%{wks.location}/Locus/vendor/entt/include"
IncludeDir["yaml_cpp"] = "%{wks.location}/Locus/vendor/yaml-cpp/include"
IncludeDir["ImGuizmo"] = "%{wks.location}/Locus/vendor/ImGuizmo"
IncludeDir["shaderc"] = "%{wks.location}/Locus/vendor/shaderc/include"
IncludeDir["SPIRV_Cross"] = "%{wks.location}/Locus/vendor/SPIRV-Cross"
IncludeDir["Box2D"] = "%{wks.location}/Locus/vendor/Box2D/include"
IncludeDir["mono"] = "%{wks.location}/Locus/vendor/mono/include"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"

LibraryDir = {}

LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
LibraryDir["VulkanSDK_Debug"] = "%{wks.location}/Locus/vendor/VulkanSDK/Lib"
LibraryDir["mono"] = "%{wks.location}/Locus/vendor/mono/lib/%{cfg.buildcfg}"

Library = {}
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/SPIRV-Toolsd.lib"

Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"

Library["mono"] = "%{LibraryDir.mono}/libmono-static-sgen.lib"

-- Windows
Library["WinSock"] = "Ws2_32.lib"
Library["WinMM"] = "Winmm.lib"
Library["WinVersion"] = "Version.lib"
Library["BCrypt"] = "Bcrypt.lib"