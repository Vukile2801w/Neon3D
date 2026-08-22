#version 420 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in vec3 a_Tangent;

out mat3 v_TBN;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;


uniform vec3 u_Color;

out vec3 v_Normal;
out vec3 FragPos;
out vec3 v_Color;
out vec2 v_TexCoord;

void main()
{
    vec4 worldPosition =
    u_Model * vec4(a_Position, 1.0);

    FragPos = worldPosition.xyz;

    mat3 normalMatrix =
        mat3(transpose(inverse(u_Model)));

    vec3 N =
        normalize(normalMatrix * a_Normal);

    vec3 T =
        normalize(normalMatrix * a_Tangent);

    T = normalize(T - dot(T, N) * N);

    vec3 B =
        normalize(cross(N, T));

    v_Normal = N;
    v_TBN = mat3(T, B, N);

    v_Color = u_Color;
    v_TexCoord = a_TexCoord;

    gl_Position =
        u_Projection *
        u_View *
        worldPosition;
}