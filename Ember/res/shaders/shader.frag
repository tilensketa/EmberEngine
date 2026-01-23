#version 460 core

layout(binding = 0) uniform sampler2D uAlbedoMap;
layout(binding = 1) uniform sampler2D uNormalMap;
layout(binding = 2) uniform sampler2D uMetalnessMap;
layout(binding = 3) uniform sampler2D uRoughnessMap;

out vec4 FragColor;

in vec3 fragPos;
in vec2 texCoords;
in mat3 TBN;

uniform int uViewMode;
uniform float uTime;
uniform vec3 uCameraPos;

uniform bool uHasAlbedoMap;
uniform bool uHasNormalMap;
uniform bool uHasMetalnessMap;
uniform bool uHasRoughnessMap;

uniform vec3 uAlbedoColor;
uniform float uEmission;
uniform float uMetalness;
uniform float uRoughness;

#define MAX_LIGHTS 128
struct Light {
    vec4 position; // xyz + type
    vec4 color; // rgb
    vec4 props; // intensity + radius
};
layout(std140, binding = 0) uniform LightBlock {
    ivec4 uLightCount;
    Light uLights[MAX_LIGHTS];
};

vec3 getNormal() {
    if (uHasNormalMap) {
        vec3 normal = texture(uNormalMap, texCoords).rgb;
        // Map from 0--1 -> -1--1
        normal = normal * 2.0 - 1.0;
        return normalize(TBN * normal);
    }
    return normalize(TBN[2]);
}
float getMetalness() {
    if (uHasMetalnessMap) {
        return texture(uMetalnessMap, texCoords).r;
    }
    return uMetalness;
}

float getRoughness() {
    if (uHasRoughnessMap) {
        return texture(uRoughnessMap, texCoords).r;
    }
    return uRoughness;
}
vec4 getMaterialColor() {
    if (uHasAlbedoMap) {
        vec4 texColor = texture(uAlbedoMap, texCoords);
        return vec4(texColor.rgb * uAlbedoColor, texColor.a);
    }
    return vec4(uAlbedoColor, 1.0);
}

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    return a2 / (PI * denom * denom);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;

    return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);

    float ggx1 = GeometrySchlickGGX(NdotV, roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 applyLightPBR(
    Light light,
    vec3 fragPos,
    vec3 N,
    vec3 V,
    vec3 albedo,
    float metalness,
    float roughness
) {
    vec3 L;
    float attenuation = 1.0;

    int type = int(light.position.w);

    if (type == 0) { // point
        vec3 toLight = light.position.xyz - fragPos;
	float dist = length(toLight);
	L = normalize(toLight);
        float att = max(1.0 - dist / light.props.g, 0.0);
        attenuation = att * att;
    } else { // directional
        L = normalize(light.position.xyz);
    }

    vec3 H = normalize(V + L);

    float NdotL = max(dot(N, L), 0.0);
    float NdotV = max(dot(N, V), 0.0);

    if (NdotL <= 0.0 || NdotV <= 0.0)
        return vec3(0.0);

    // Base reflectivity
    vec3 F0 = mix(vec3(0.04), albedo, metalness);

    // Cook-Torrance BRDF
    float D = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator = D * G * F;
    float denominator = 4.0 * NdotV * NdotL + 0.0001;
    vec3 specular = numerator / denominator;

    // Energy conservation
    vec3 kS = F;
    vec3 kD = (vec3(1.0) - kS) * (1.0 - metalness);

    vec3 diffuse = kD * albedo / PI;

    vec3 lightColor = light.color.rgb * light.props.r;

    return (diffuse + specular) * lightColor * NdotL * attenuation;
}

vec3 lighting(
    vec3 fragPos,
    vec3 normal,
    vec3 albedo,
    float metalness,
    float roughness
) {
    vec3 viewDir = normalize(uCameraPos - fragPos);
    vec3 result = vec3(0.0);

    for (int i = 0; i < uLightCount.x; ++i) {
        result += applyLightPBR(
                uLights[i],
                fragPos,
                normal,
                viewDir,
                albedo,
                metalness,
                roughness
            );
    }

    // Ambient
    result += albedo * 0.1;

    return result;
}

void main()
{
    vec3 normal = getNormal();
    vec4 matColor = getMaterialColor();

    float metalness = getMetalness();
    float roughness = clamp(getRoughness(), 0.04, 1.0);

    vec3 litSolid = lighting(fragPos, normal, vec3(1.0f), metalness, roughness);
    vec3 litColor = lighting(fragPos, normal, matColor.rgb, metalness, roughness);
    vec3 emission = matColor.rgb * uEmission;

    if (uViewMode == 0) {
        FragColor = vec4(0, 0, 0, 1);
    } else if (uViewMode == 1) {
        FragColor = vec4(litSolid, 1.0);
    } else if (uViewMode == 2) {
        FragColor = matColor;
    } else if (uViewMode == 3) {
        FragColor = vec4(normal * 0.5 + 0.5, 1.0);
    } else if (uViewMode == 4) {
        FragColor = vec4(vec3(metalness), 1.0);
    } else if (uViewMode == 5) {
        FragColor = vec4(vec3(roughness), 1.0);
    } else if (uViewMode == 6) {
        FragColor = vec4(texCoords, 0, 1);
    } else if (uViewMode == 7) {
        FragColor = vec4(litColor + emission, matColor.a);
    }
}
