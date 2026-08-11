#version 330 core

layout(location = 0) in vec3 a_Position;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

uniform float u_time;

out vec3 v_WorldPosition;

void main()
{
    vec4 worldPosition = u_Model * vec4(a_Position, 1.0);

    // Blago "življenje" kocke
    worldPosition.y +=
        sin(a_Position.x * 4.0 + u_time * 2.0) * 0.025;

    v_WorldPosition = worldPosition.xyz;

    gl_Position =
        u_Projection *
        u_View *
        worldPosition;
}