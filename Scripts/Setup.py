import os
import subprocess

# Change from Scripts directory to root
os.chdir('../')

import SetupPython
SetupPython.ValidatePackages()

import SetupVulkan
SetupVulkan.ValidatePackages()


import SetupPremake
SetupPremake.ValidatePackages()

print("Running premake...")
subprocess.call(["vendor/premake/bin/premake5.exe", "vs2022"])
print("Finished setting up SideA Engine.")