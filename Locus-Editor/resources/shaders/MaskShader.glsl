// --- MaskShader -------------------------------------------------------------
// Draws a white mask of the mesh.

// --- Vertex Shader ---
#type vertex
#version 450 core

layout (location = 0) in vec3 a_Position;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
	vec4 u_CameraPosition;
	vec2 u_ViewportSize;
};

void main()
{
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0f);
}



// --- Fragment Shader ---
#type fragment
#version 450 core

layout (location = 0) out int o_Color;

void main()
{
	o_Color = 1;
}