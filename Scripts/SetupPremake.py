import os

import Utils

from io import BytesIO
from urllib.request import urlopen
from zipfile import ZipFile

PREMAKE_VERSION = "5.0.0-beta2"
PREMAKE_ZIP_URL = f"https://github.com/premake/premake-core/releases/download/v{PREMAKE_VERSION}/premake-{PREMAKE_VERSION}-windows.zip"
PREMAKE_LICENSE_URL = "https://raw.githubusercontent.com/premake/premake-core/master/LICENSE.txt"
PREMAKE_DIR_PATH = "vendor/premake/bin"
PREMAKE_EXE_PATH = "vendor/premake/bin/premake5.exe"

def ValidatePackages():
    if not CheckIfPremakeInstalled():
        print("Premake is not installed.")
        return False
    
    print(f"Correct Premake located at {os.path.abspath(PREMAKE_EXE_PATH)}")
    return True

def CheckIfPremakeInstalled():
    if not os.path.exists(PREMAKE_EXE_PATH):
        return InstallPremake()
    return True

def InstallPremake():
    if not os.path.exists(PREMAKE_DIR_PATH):
        os.makedirs(PREMAKE_DIR_PATH)
    print("Would you like to install premake?")
    install = Utils.YesOrNo()
    if install:
        premakePath = f"{PREMAKE_DIR_PATH}/premake-{PREMAKE_VERSION}-windows.zip"
        print("Downloading {0:s} to {1:s}".format(PREMAKE_ZIP_URL, PREMAKE_DIR_PATH))
        Utils.DownloadFile(PREMAKE_ZIP_URL, premakePath)
        print("Extracting", premakePath)
        Utils.UnzipFile(premakePath, deleteZipFile=True)
        print(f"Premake {PREMAKE_VERSION} has been downloaded to '{PREMAKE_DIR_PATH}'")

        premakeLicensePath = f"{PREMAKE_DIR_PATH}/LICENSE.txt"
        print("Downloading {0:s} to {1:s}".format(PREMAKE_LICENSE_URL, premakeLicensePath))
        Utils.DownloadFile(PREMAKE_LICENSE_URL, premakeLicensePath)
        print(f"Premake License file has been downloaded to '{PREMAKE_DIR_PATH}'")

    return True
