#version 330 core

in vec3 v_WorldPosition;

uniform float u_time;
uniform vec3 u_color;

out vec4 FragColor;

void main()
{
    // Horizontalne skenirajuće linije
    float scan = sin(
        v_WorldPosition.y * 35.0 -
        u_time * 5.0
    );

    scan = scan * 0.5 + 0.5;

    // Pulsiranje
    float pulse =
        sin(u_time * 3.0) * 0.5 + 0.5;

    // Kombinacija
    float intensity =
        0.35 +
        scan * 0.35 +
        pulse * 0.2;

    vec3 color =
        u_color * intensity;

    FragColor = vec4(color, 1.0);
}