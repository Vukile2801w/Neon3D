#version 420 core

in vec3 v_Normal;
in vec3 FragPos;
in vec2 v_TexCoord;
in mat3 v_TBN;

uniform sampler2D T_Color;
uniform sampler2D T_Normal;

uniform vec3 u_ViewPos;

uniform vec3 u_Color;
uniform float u_Shininess;
uniform bool u_IsLightSource;
uniform bool u_HasTexture;

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


vec3 calculateLight(Light light)
{
    // Normal map
    vec3 norm = texture(T_Normal, v_TexCoord).rgb;

    norm = norm * 2.0 - 1.0;
    norm = normalize(v_TBN * norm);

    vec3 toLight = light.pos - FragPos;

    float distance = length(toLight);

    vec3 lightDir = normalize(toLight);

    float diff = max(dot(norm, lightDir), 0.0);

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

    // Albedo
    vec3 albedo;

    if (u_HasTexture)
    {
        albedo =
            texture(T_Color, v_TexCoord).rgb;
    }
    else
    {
        albedo = u_Color;
    }

    return diffuse * albedo + specular;
}


void main()
{
    if (dot(v_Normal, u_ViewPos - FragPos) < 0)
        return;

    if (u_IsLightSource)
    {
        FragColor = vec4(u_Color, 1.0);
        return;
    }

    // ==================================
    // Albedo
    // ==================================

    vec3 albedo;

    if (u_HasTexture)
    {
        albedo =
            texture(T_Color, v_TexCoord).rgb;
    }
    else
    {
        albedo = u_Color;
    }

    // ==================================
    // Ambient
    // ==================================

    float ambient = 0.3;

    vec3 result =
        ambient * albedo;

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