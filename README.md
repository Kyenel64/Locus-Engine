# Locus Game Engine
<p align="center">
  <img src="./Resources/Branding/Logo/LocusLogoWithName.png" alt="Locus Logo" width="800"/>
</p>

Locus is an open-source, lightweight, general-purpose game engine for Windows.
## Links
[Website & Documentation](https://www.locusengine.com)

## Progress
Locus is still in early stages of development and the focus right now is on the 2D features and implementing basic engine features. 
Once Locus has a solid 2D workflow, implementation for 3D features will begin.
Progress for Locus can be followed here: https://trello.com/b/NNDPkCjF/locus-kanban

## Features for Locus 2D
- Integrated editor
- C# scripting
- 2D physics system
- Sprite rendering
- Circle rendering
- UI rendering (TODO)
- Spritesheets (TODO)
- Skeletal animations (TODO)
- 2D particle system (TODO)
- Audio system (TODO)

## Locus Editor
<p>
  <img src="./Resources/Branding/DemoScene.png" alt="Locus Editor" width="800"/>
</p>

## How to Build
Requirements: Visual Studio 2022, Python3

#### 1. Downloading source code
- Download the zip and extract the files or use `git clone --recurse-submodules https://github.com/kyenel64/Locus-Engine` in command prompt.

#### 1.1 (Temp) Build SandboxProject
Locus currently does not have a project directory system setup. There is a placeholder project directory called SandboxProject within the Locus-Editor directory.
You will need to manually generate and build the project solution for now.
- Run Locus-Editor/SandboxProject/Setup.bat.
- Open Locus-Editor/SandboxProject/Sandbox.sln.
- Build solution.

#### 2. Run Scripts/Setup.bat
- Run Scripts/Setup.bat to install dependencies and generate a Visual Studio solution.
- Temp: Run Locus-Editor/SandboxProject/Setup.bat. (This is temporary until a proper project directory system is implemented.)
- IMPORTANT: Make sure 'Shader Toolchain Debug Symbols - 64-bit' is selected when the Vulkan pop-up opens.

#### 3. Build using Visual Studio
- Build solution using Visual Studio (Locus-Editor should be selected as the startup project).
- Temp: Build Locus-Editor/SandboxProject/Sandbox.sln. 

#### 4. Running the engine editor
- Press F5 or click the start button in Visual Studio to run the application.

## Dependencies
- [GLFW](https://github.com/glfw/glfw)
- [Glad](https://github.com/Dav1dde/glad)
- [premake](https://github.com/premake/premake-core)
- [VulkanSDK](https://www.vulkan.org/)
- [ImGui](https://github.com/ocornut/imgui)
- [Entt](https://github.com/skypjack/entt)
- [Box2D](https://github.com/erincatto/box2d)
- [glm](https://github.com/g-truc/glm)
- [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo)
- [mono](https://www.mono-project.com/)
- [spdlog](https://github.com/gabime/spdlog)
- [stb_image](https://github.com/nothings/stb)
- [yaml-cpp](https://github.com/jbeder/yaml-cpp)
