#version 420 core

in vec3 v_Normal;
in vec3 FragPos;
in vec3 v_Color;

out vec4 FragColor;

struct Light
{
    vec3 pos;
    vec3 color;
};

#define MAX_LIGHTS 32

uniform Light lights[MAX_LIGHTS];
uniform int lightCount;

vec3 calculateLight(Light light)
{
    vec3 norm = normalize(v_Normal);

    vec3 lightDir =
        normalize(light.pos - FragPos);

    float diff =
        max(dot(norm, lightDir), 0.0);

    return diff * light.color;
}

void main()
{
    // Jedan ambient za celu scenu
    float ambient = 0.3f;

    vec3 result =
        ambient * v_Color;

    // Sva svetla
    for (int i = 0; i < lightCount; ++i)
    {
        result +=
            calculateLight(lights[i]) * v_Color;
    }

    FragColor = vec4(result, 1.0);
}