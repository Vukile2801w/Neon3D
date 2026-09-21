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

// Set manually per-material via Material::setProperty("u_IsLightSource", true) when
// a mesh should render as a flat, unlit color instead of being shaded (e.g. a
// visual marker placed at a light's position). No longer driven by the scene
// Light system or SceneSerializer - Light is fully decoupled from Material/
// GameObject (see Rendering/Light.hpp) and never sets this uniform itself.
uniform bool u_IsLightSource;
uniform bool u_HasTexture;
uniform bool u_HasNormalTexture;

out vec4 FragColor;

struct Light
{
    // Selects which of the fields below are meaningful for this light and which
    // branch of getLightVector() below computes its direction/falloff. Values
    // must match the LIGHT_TYPE_* constants and Renderer.cpp's ShaderLightType
    // enum - see the comment on getLightVector() for how to add a new type.
    int type;

    vec3 position;   // Point, Spot - world-space light position. Unused by Directional.
    vec3 direction;  // Directional, Spot - world-space direction the light travels *in*
                      // (i.e. surface receives light from -direction). Unused by Point.

    vec3 color;
    float intensity;

    float range;         // Point, Spot - distance at which the light fades to nothing.
    float innerConeDeg;   // Spot only - full brightness inside this half-angle.
    float outerConeDeg;   // Spot only - zero brightness outside this half-angle.
};

#define LIGHT_TYPE_POINT       0
#define LIGHT_TYPE_DIRECTIONAL 1
#define LIGHT_TYPE_SPOT        2

#define MAX_LIGHTS 64

uniform Light lights[MAX_LIGHTS];
uniform int lightCount;


// ==================================
// Albedo
// ==================================

vec3 getAlbedo()
{
    if (u_HasTexture)
        return texture(T_Color, v_TexCoord).rgb;

    return u_Color;
}


// ==================================
// Normal
// ==================================

vec3 getNormal()
{
    // Default: geometry normal
    vec3 normal =
        normalize(v_Normal);

    // Override with normal map when available
    if (u_HasNormalTexture)
    {
        vec3 normalMap =
            texture(T_Normal, v_TexCoord).rgb;

        // [0, 1] -> [-1, 1]
        normalMap =
            normalMap * 2.0 - 1.0;

        // Tangent space -> world space
        normal =
            normalize(v_TBN * normalMap);
    }

    return normal;
}


// ==================================
// Lighting
// ==================================

// Works out (a) the direction from the surface toward the light and (b) how much
// of the light reaches this point at all (0 = none, 1 = full) - the only two
// things that actually differ between light types. Everything else in
// calculateLight() below (diffuse/specular) is shared and never needs to change.
//
// To add a new light type: add a LIGHT_TYPE_* constant above, add an "if" branch
// here that fills in lightDir/attenuation for it, and set Light.type to it from
// Renderer.cpp. Nothing else in this file needs to change.
void getLightVector(
    Light light,
    out vec3 lightDir,
    out float attenuation
)
{
    if (light.type == LIGHT_TYPE_DIRECTIONAL)
    {
        // No position, no falloff - a directional light is treated as infinitely
        // far away, arriving everywhere from the same direction.
        lightDir = normalize(-light.direction);
        attenuation = 1.0;
        return;
    }

    // Point and Spot both live at a position in the scene and fade out over
    // 'range' - this part is shared between them.
    vec3 toLight =
        light.position - FragPos;

    float distance =
        length(toLight);

    lightDir =
        normalize(toLight);

    attenuation =
        clamp(1.0 - distance / max(light.range, 0.0001), 0.0, 1.0);

    // Smoother falloff than a linear one, without needing extra light data.
    attenuation *= attenuation;

    if (light.type == LIGHT_TYPE_SPOT)
    {
        // How far off-axis the fragment is from the spot's own forward direction,
        // as a cosine (cheaper to compare than converting back to an angle).
        float cosAngle =
            dot(-lightDir, normalize(light.direction));

        float innerCos = cos(radians(light.innerConeDeg));
        float outerCos = cos(radians(light.outerConeDeg));

        // 1.0 inside the inner cone, 0.0 outside the outer cone, smooth between.
        attenuation *= smoothstep(outerCos, innerCos, cosAngle);
    }
}

vec3 calculateLight(
    Light light,
    vec3 normal,
    vec3 albedo
)
{
    vec3 lightDir;
    float attenuation;
    getLightVector(light, lightDir, attenuation);

    // Fully outside the light's reach (too far / outside the spot cone) - skip the
    // shading math entirely.
    if (attenuation <= 0.0)
        return vec3(0.0);

    // Diffuse
    float diff =
        max(dot(normal, lightDir), 0.0);
        
    vec3 diffuse =
        diff *
        light.color *
        light.intensity *
        attenuation;

    // Specular
    vec3 viewDir =
        normalize(u_ViewPos - FragPos);

    vec3 reflectDir =
        reflect(-lightDir, normal);

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


    return diffuse * albedo + specular;
}


// ==================================
// Main
// ==================================

void main()
{
    if (u_IsLightSource)
    {
        FragColor =
            vec4(u_Color, 1.0);

        return;
    }

    vec3 albedo =
        getAlbedo();

    vec3 normal =
        getNormal();

    // Ambient
    float ambient =
        0.2;

    vec3 result =
        ambient * albedo;

    // Lights
    for (int i = 0; i < lightCount; ++i)
    {
        result +=
            calculateLight(
                lights[i],
                normal,
                albedo
            );
    }

    FragColor =
        vec4(result, 1.0);
}