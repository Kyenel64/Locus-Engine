import os
import subprocess
import sys
from pathlib import Path
import shutil

import Utils

from io import BytesIO
from urllib.request import urlopen
from zipfile import ZipFile


VULKAN_SDK = os.environ.get('VULKAN_SDK')
VULKAN_SDK_INSTALLER_URL = 'https://sdk.lunarg.com/sdk/download/1.3.250.1/windows/VulkanSDK-1.3.250.1-Installer.exe'
LOCUS_VULKAN_VERSION = '1.3.250.1'
VULKAN_SDK_EXE_PATH = 'Locus/vendor/VulkanSDK/VulkanSDK.exe'

def ValidatePackages():
    if (not CheckVulkanSDK()):
        print("Vulkan SDK not installed.")

    if (not CheckVulkanSDKDebugLibs()):
        print("Vulkan SDK debug libs not found.")

def InstallVulkanSDK():
    print('Downloading {} to {}'.format(VULKAN_SDK_INSTALLER_URL, VULKAN_SDK_EXE_PATH))
    Utils.DownloadFile(VULKAN_SDK_INSTALLER_URL, VULKAN_SDK_EXE_PATH)
    print("Done!")
    print("Running Vulkan SDK installer...")
    print("MAKE SURE 'Shader Toolchain Debug Symbols - 64-bit' IS SELECTED")
    os.startfile(os.path.abspath(VULKAN_SDK_EXE_PATH))
    print("Re-run this script after installation")


def InstallVulkanPrompt():
    if not os.path.exists('Locus/vendor/VulkanSDK'):
        os.makedirs('Locus/vendor/VulkanSDK')
    print("Would you like to install the Vulkan SDK?")
    install = Utils.YesOrNo()
    if (install):
        InstallVulkanSDK()
        quit()

def CheckVulkanSDK():
    if (VULKAN_SDK is None):
        print("You don't have the Vulkan SDK installed!")
        InstallVulkanPrompt()
        return False
    elif (LOCUS_VULKAN_VERSION not in VULKAN_SDK):
        print(f"Located Vulkan SDK at {VULKAN_SDK}")
        print(f"You don't have the correct Vulkan SDK version! (Locus requires {LOCUS_VULKAN_VERSION})")
        InstallVulkanPrompt()
        return False

    print(f"Correct Vulkan SDK located at {VULKAN_SDK}")
    return True

VulkanSDKDebugLibsURL = 'https://files.lunarg.com/SDK-1.2.170.0/VulkanSDK-1.2.170.0-DebugLibs.zip'
OutputDirectory = "Locus/vendor/VulkanSDK"
TempZipFile = f"{OutputDirectory}/VulkanSDK.zip"

def CheckVulkanSDKDebugLibs():
    if not os.path.exists("Locus/vendor/VulkanSDK/Lib"):
        origin = f"{VULKAN_SDK}/Lib"
        target = f"{OutputDirectory}/Lib"
        shutil.copytree(origin, target)

    shadercdLib = Path(f"{OutputDirectory}/Lib/shaderc_sharedd.lib")
    if (not shadercdLib.exists()):
        print(f"No Vulkan SDK debug libs found. (Checked {shadercdLib})")
        print("Downloading", VulkanSDKDebugLibsURL)
        with urlopen(VulkanSDKDebugLibsURL) as zipresp:
            with ZipFile(BytesIO(zipresp.read())) as zfile:
                zfile.extractall(OutputDirectory)

    print(f"Vulkan SDK debug libs located at {OutputDirectory}")
    return True