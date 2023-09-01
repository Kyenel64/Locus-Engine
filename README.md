# Locus Game Engine
Locus is a 2D/3D game engine currently in development for Windows. Currently only supports Windows with OpenGL but other platforms/APIs will be implemented in the future.  
Trello: https://trello.com/b/NNDPkCjF/locus-kanban
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
- Press F5 or click the start button in Visual Studio to run the application.
