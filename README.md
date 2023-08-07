# Locus Engine
Locus is a game engine currently in development for Windows using C++ and OpenGL. More platforms and rendering APIs may be implemented in the future.
***
## How to Install

Requirements: Visual Studio 2022

#### 1. Downloading source code
- Download the zip and extract the files or use `git clone --recurse-submodules https://github.com/kyenel64/Locus-Engine` in command prompt.

#### 2. Run Scripts/Setup.bat
- Run Scripts/Setup.bat to install dependencies and generate a Visual Studio solution.
- IMPORTANT: Make sure 'Shader Toolchain Debug Symbols - 64-bit' is selected when installing Vulkan.

#### 3. Build using Visual Studio
- Build solution using Visual Studio (Locus-Editor should be selected as the startup project).

#### 4. Running the engine editor
- You can either run using Visual Studio or the generated .exe.
- The .exe will be generated in `bin/(Config)-windows-x86_64/Locus-Editor/Locus-Editor.exe`.
- To run using the .exe file, copy the `assets` folder from `Locus-Editor/` to the directory containing .exe.
