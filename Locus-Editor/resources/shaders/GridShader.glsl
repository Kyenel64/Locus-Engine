// --- Renderer2D grid shader -------------------------------------------------

// --- Vertex Shader ---
#type vertex
#version 450 core

layout(location = 0) in int a_Index;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
};

layout(location = 0) out vec3 v_NearPoint;
layout(location = 1) out vec3 v_FarPoint;
layout(location = 2) out mat4 v_ViewProj;

// Grid position are in clipped space
vec3 gridPlane[4] = vec3[]
(
    vec3(1, 1, 0), vec3(-1, 1, 0), vec3(-1, -1, 0),
    vec3(1, -1, 0)
);

vec3 UnprojectPoint(float x, float y, float z, mat4 viewProjection)
{
    mat4 viewProjInv = inverse(viewProjection);
    vec4 unprojectedPoint = viewProjInv * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main()
{
	v_ViewProj = u_ViewProjection;

	vec3 p = gridPlane[a_Index].xyz;
	v_NearPoint = UnprojectPoint(p.x, p.y, 0.0, u_ViewProjection).xyz; // unprojecting on the near plane
	v_FarPoint = UnprojectPoint(p.x, p.y, 1.0, u_ViewProjection).xyz; // unprojecting on the far plane

	gl_Position = vec4(p, 1.0f);
}


// --- Fragment Shader ---
#type fragment
#version 450 core

layout(location = 0) in vec3 v_NearPoint;
layout(location = 1) in vec3 v_FarPoint;
layout(location = 2) in mat4 v_ViewProj;

layout(location = 0) out vec4 o_Color;

vec4 grid(vec3 fragPos3D, float scale)
{
    vec2 coord = fragPos3D.xz * scale;
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);
    vec4 color = vec4(0.8f, 0.8f, 0.8f, 1.0 - min(line, 1.0));
    return color;
}

float computeDepth(vec3 pos)
{
    vec4 clip_space_pos = v_ViewProj * vec4(pos.xyz, 1.0);
    return 0.5f + 0.5f * (clip_space_pos.z / clip_space_pos.w);
}

float computeLinearDepth(vec3 pos)
{
    vec4 clip_space_pos = v_ViewProj * vec4(pos.xyz, 1.0);
    // float clip_space_depth = (clip_space_pos.z / clip_space_pos.w) * 2.0 - 1.0; // put back between -1 and 1
    float clip_space_depth = 0.5f + 0.5f * (clip_space_pos.z / clip_space_pos.w);
    float linearDepth = (2.0 * 0.01 * 1000) / (1000 + 0.01 - clip_space_depth * (1000 - 0.01)); // get linear value between 0.01 and 100
    return linearDepth / 1000; // normalize
}

void main()
{
	float t = -v_NearPoint.y / (v_FarPoint.y - v_NearPoint.y);
	vec3 fragPos3D = v_NearPoint + t * (v_FarPoint - v_NearPoint);

	gl_FragDepth = computeDepth(fragPos3D);

	float linearDepth = computeLinearDepth(fragPos3D);
	float fading = max(0, (0.5 - linearDepth));

    vec4 grid1 = grid(fragPos3D, 0.01f);
    vec4 grid2 = grid(fragPos3D, 0.1f);
    vec4 grid3 = grid(fragPos3D, 1);
	o_Color = (grid1 + grid2 + grid3) * float(t > 0); // adding multiple resolution for the grid
    o_Color.a *= fading - 0.35f;
}