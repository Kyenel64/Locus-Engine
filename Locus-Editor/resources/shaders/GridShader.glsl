// --- Renderer2D grid shader -------------------------------------------------

// --- Vertex Shader ---
#type vertex
#version 450 core

layout(location = 0) in int a_Index;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_View;
    mat4 u_Projection;
    vec4 u_CameraPosition;
    vec2 u_ViewportSize;
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
	v_ViewProj = u_Projection * u_View;

	vec3 p = gridPlane[a_Index].xyz;
	v_NearPoint = UnprojectPoint(p.x, p.y, 0.0, u_Projection * u_View).xyz; // unprojecting on the near plane
	v_FarPoint = UnprojectPoint(p.x, p.y, 1.0, u_Projection * u_View).xyz; // unprojecting on the far plane

	gl_Position = vec4(p, 1.0f);
}


// --- Fragment Shader ---
#type fragment
#version 450 core

layout(location = 0) in vec3 v_NearPoint;
layout(location = 1) in vec3 v_FarPoint;
layout(location = 2) in mat4 v_ViewProj;

layout(std140, binding = 1) uniform Grid
{
    vec4 u_Color;
    float u_Near;
    float u_Far;
    float u_GridScale;
};

layout(location = 0) out vec4 o_Color;

vec4 grid(vec3 fragPos3D, float scale)
{
    vec2 coord = fragPos3D.xz * scale;
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);
    vec4 color = vec4(u_Color.xyz, 1.0 - min(line, 1.0));
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
    float linearDepth = (2.0 * u_Near * u_Far) / (u_Far + u_Near - clip_space_depth * (u_Far - u_Near)); // get linear value between 0.01 and 100
    return linearDepth / u_Far; // normalize
}

void main()
{
	float t = -v_NearPoint.y / (v_FarPoint.y - v_NearPoint.y);
	vec3 fragPos3D = v_NearPoint + t * (v_FarPoint - v_NearPoint);

	gl_FragDepth = computeDepth(fragPos3D);

	float linearDepth = computeLinearDepth(fragPos3D);
	float fading = max(0, (0.5 - linearDepth));

    o_Color += grid(fragPos3D, u_GridScale);
    for (int i = 1; i < 5; i++)
    {
        o_Color += grid(fragPos3D, u_GridScale / (pow(10, i)));
    }

	o_Color *= float(t > 0);
    o_Color.a *= fading - 0.35f;
}