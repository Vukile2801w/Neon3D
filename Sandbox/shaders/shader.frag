#version 420 core

out vec4 FragColor;  
in vec3 ourColor;

uniform float time;

void main()
{
    float mul = sin(time);
    if (mul < 0)
        mul *= -1;

    FragColor = vec4(ourColor * mul, 1.0);

}