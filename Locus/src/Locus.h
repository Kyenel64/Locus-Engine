// --- Locus -------------------------------------------------------------------
// Main include file for client applications.
#pragma once

// --- Core ---
#include "Locus/Core/Application.h"
#include "Locus/Core/Core.h"
#include "Locus/Core/Log.h"
#include "Locus/Core/Layer.h"
#include "Locus/Core/Input.h"
#include "Locus/Core/KeyCodes.h"
#include "Locus/Core/MouseCodes.h"
#include "Locus/Core/Timestep.h"
#include "Locus/Core/Timer.h"

// --- Debug ---
#include "Locus/Debug/Instrumentor.h"

// --- ImGui ---
#include "Locus/ImGui/ImGuiLayer.h"

// --- Math ---
#include "Locus/Math/Math.h"

// --- Renderer ---
#include "Locus/Renderer/Renderer.h"
#include "Locus/Renderer/Renderer2D.h"
#include "Locus/Renderer/Renderer3D.h"
#include "Locus/Renderer/RendererStats.h"
#include "Locus/Renderer/RendererAPI.h"
#include "Locus/Renderer/RenderCommand.h"
#include "Locus/Renderer/Buffer.h"
#include "Locus/Renderer/VertexArray.h"
#include "Locus/Renderer/Shader.h"
#include "Locus/Renderer/Texture.h"
#include "Locus/Renderer/SubTexture2D.h"
#include "Locus/Renderer/Framebuffer.h"
#include "Locus/Renderer/EditorCamera.h"

// --- Scene ---
#include "Locus/Scene/Scene.h"
#include "Locus/Scene/Entity.h"
#include "Locus/Scene/Components.h"
#include "Locus/Scene/SceneCamera.h"
#include "Locus/Scene/SceneSerializer.h"

// --- Scripting ---
#include "Locus/Scripting/ScriptEngine.h"

// --- Utils ---
#include "Locus/Utils/PlatformUtils.h"
