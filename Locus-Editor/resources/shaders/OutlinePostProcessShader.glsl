// --- OutlinePostProcessShader -----------------------------------------------
// Edge detection post process for outlines

// --- Vertex ---
#type vertex
#version 450 core

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec2 a_TexCoord;

layout(location = 0) out vec2 v_TexCoord;
layout(location = 1) out vec2 v_FragCoord;

void main() {
	v_TexCoord = a_TexCoord;
	v_FragCoord = gl_Position.xy;
	gl_Position = vec4(a_Position, 0.0f, 1.0f);
}


// --- Fragment ---
#type fragment
#version 450 core

layout(location = 0) in vec2 v_TexCoord;
layout(location = 1) in vec2 v_FragCoord;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
	vec4 u_CameraPosition;
	vec2 u_ViewportSize;
};

layout(binding = 0) uniform isampler2D u_Texture;

layout(location = 0) out vec4 o_Color;

void main()
{
	float offsetX = 1.0f / u_ViewportSize.x;
	float offsetY = 1.0f / u_ViewportSize.y;
	vec2 offsets[9] = vec2[]
	(
		vec2(-offsetX,  offsetY), // top-left
		vec2( 0.0f,     offsetY), // top-center
		vec2( offsetX,  offsetY), // top-right
		vec2(-offsetX,  0.0f),    // center-left
		vec2( 0.0f,     0.0f),    // center-center
		vec2( offsetX,  0.0f),    // center-right
		vec2(-offsetX, -offsetY), // bottom-left
		vec2( 0.0f,    -offsetY), // bottom-center
		vec2( offsetX, -offsetY)  // bottom-right
	);

	int kernel[9] = int[]
	(
		1,  1,  1,
		1, -8,  1,
		1,  1,  1
	);
	
	ivec3 sampleTex[9];
	for(int i = 0; i < 9; i++)
	{
		sampleTex[i] = ivec3(texture(u_Texture, v_TexCoord.xy + offsets[i]));
	}
	ivec3 col = ivec3(0);
	for(int i = 0; i < 9; i++)
		col += sampleTex[i] * kernel[i];
	
	vec3 outCol = vec3(0.0f);
	if (col == ivec3(0))
		discard;
	else
		outCol = vec3(0.0f, 1.0f, 0.0f);

	// --- Output ---
	o_Color = vec4(outCol, 1.0);
} 
