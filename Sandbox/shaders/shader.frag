#version 420 core

in vec3 v_Normal;
in vec3 FragPos;
in vec3 v_Color;

out vec4 FragColor;

struct Light
{
    vec3 pos;
    vec3 color;
    float intensity;
};

#define MAX_LIGHTS 32

uniform Light lights[MAX_LIGHTS];
uniform int lightCount;

uniform vec3 u_ViewPos;

uniform float u_Shininess;
uniform bool u_IsLightSource;

vec3 calculateLight(Light light)
{
    vec3 norm =
        normalize(v_Normal);

    // ==================================
    // Direction to light
    // ==================================

    vec3 toLight =
        light.pos - FragPos;

    float distance =
        length(toLight);

    vec3 lightDir =
        normalize(toLight);

    // ==================================
    // Diffuse
    // ==================================

    float diff =
        max(dot(norm, lightDir), 0.0);

    // ==================================
    // Attenuation
    // ==================================

    float attenuation =
        1.0 /
        (
            1.0 +
            0.09 * distance +
            0.032 * distance * distance
        );

    vec3 diffuse =
        diff *
        light.color *
        light.intensity *
        attenuation;

    // ==================================
    // Specular
    // ==================================

    vec3 viewDir =
        normalize(u_ViewPos - FragPos);

    vec3 reflectDir =
        reflect(-lightDir, norm);

    float spec =
        pow(
            max(dot(viewDir, reflectDir), 0.0),
            u_Shininess
        );

    vec3 specular =
        spec *
        light.color *
        light.intensity *
        attenuation;

    return diffuse * v_Color + specular;
}

void main()
{
    if (u_IsLightSource){
        FragColor = vec4(v_Color, 1.0f);
        return;
    }

    // ==================================
    // Ambient
    // ==================================

    float ambient = 0.3;

    vec3 result =
        ambient * v_Color;

    // ==================================
    // Lights
    // ==================================

    for (int i = 0; i < lightCount; ++i)
    {
        result +=
            calculateLight(lights[i]);
    }

    FragColor =
        vec4(result, 1.0);
}