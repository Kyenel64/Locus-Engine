import os
import subprocess
import CheckPython

# Make sure everything we need is installed
CheckPython.ValidatePackages()

if not os.path.exists('../SideA/vendor/VulkanSDK'):
    os.makedirs('../SideA/vendor/VulkanSDK')

import Vulkan

# Change from Scripts directory to root
os.chdir('../')

if (not Vulkan.CheckVulkanSDK()):
    print("Vulkan SDK not installed.")

if (not Vulkan.CheckVulkanSDKDebugLibs()):
    print("Vulkan SDK debug libs not found.")

print("Running premake...")
subprocess.call(["vendor/premake/premake5.exe", "vs2022"])