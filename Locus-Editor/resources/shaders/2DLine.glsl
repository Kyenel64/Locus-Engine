// --- Renderer2D line shader -------------------------------------------------

// --- Vertex ---
#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in int a_EntityID;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
	vec4 u_CameraPosition;
};

layout(location = 0) out vec4 v_Color;
layout(location = 1) out flat int v_EntityID;

void main()
{
	v_Color = a_Color;
	v_EntityID = a_EntityID;
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0f);
}


// --- Fragment ---
#type fragment
#version 450 core

layout(location = 0) in vec4 v_Color;
layout(location = 1) in flat int v_EntityID;

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

void main()
{
	o_Color = v_Color;
	o_EntityID = v_EntityID;
}