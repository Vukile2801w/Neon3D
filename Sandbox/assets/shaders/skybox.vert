#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in vec3 a_Tangent;

out vec2 v_TexCoord;

uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
    v_TexCoord = a_TexCoord;

    // Skybox se renderuje bez translacije kamere.
    vec4 position = u_Projection * u_View * vec4(a_Position, 1.0);

    // Uvek ga stavi na far plane.
    gl_Position = position.xyww;
}