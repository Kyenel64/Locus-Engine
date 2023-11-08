// --- Renderer2D grid shader -------------------------------------------------

// --- Vertex Shader ---
#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
};

layout(location = 0) out vec2 v_TexCoord;

void main()
{
	v_TexCoord = a_TexCoord;
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0f);
}


// --- Fragment Shader ---
#type fragment
#version 450 core

layout(location = 0) in vec2 v_TexCoord;

layout(location = 0) out vec4 o_Color;

float filteredGrid( vec2 p, vec2 dpdx, vec2 dpdy, float width )
{
	const float N = 50;
	vec2 w = max(abs(dpdx), abs(dpdy));
	vec2 a = p + 0.5 * w;
	vec2 b = p - 0.5 * w;
	vec2 i = (floor(a) + min(fract(a) * N * width, 1.0) - floor(b) - min(fract(b) * N * width, 1.0)) / (N * w);
	return (1.0 - i.x) * (1.0 - i.y);
}

void main()
{
	vec2 uv = v_TexCoord * 10;
	float grid = 1 - filteredGrid(uv, dFdx(uv), dFdy(uv), 1000);

	vec2 uv2 = uv * 10;
	float grid2 = 1 - filteredGrid(uv2, dFdx(uv2), dFdy(uv2), 1000);

	vec2 uv3 = uv2 * 10;
	float grid3 = 1 - filteredGrid(uv3, dFdx(uv3), dFdy(uv3), 1000);

	float fullGrid = grid + grid2 + grid3;

	vec4 col = vec4(fullGrid);

	o_Color = col;
}