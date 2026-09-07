#version 460 core

in vec2 v_TexCoord;

uniform sampler2D u_Skybox;

out vec4 FragColor;

void main()
{
    FragColor = texture(u_Skybox, v_TexCoord);
}