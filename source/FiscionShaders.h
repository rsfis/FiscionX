const char* uiVertex = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
uniform mat4 uMVP;
void main() {
    gl_Position = uMVP * vec4(aPos, 0.0, 1.0);
}
)";

const char* uiFragment = R"(
#version 330 core
out vec4 FragColor;
uniform vec4 uColor;
void main() {
    FragColor = uColor;
}
)";

const char* vertexDebug = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 fragColor;

uniform mat4 viewProj;

void main() {
    fragColor = aColor;
    gl_Position = viewProj * vec4(aPos, 1.0);
}
)";

const char* fragmentDebug = R"(
#version 330 core
in vec3 fragColor;
out vec4 color;

void main() {
    color = vec4(fragColor, 1.0);
}
)";

const char* videoVertex = R"(
#version 330 core
layout (location = 0) in vec2 aPos; // -1 .. 1
layout (location = 1) in vec2 aTex;

out vec2 TexCoord;

uniform vec2 position;    // CENTER of video in pixels (x, y)
uniform vec2 scale;       // size: width, height in pixels (width, height)
uniform float rotation;   // radians
uniform mat4 projection;  // use: glm::ortho(-W/2, W/2, -H/2, H/2) => screen center = (0,0)

void main() {
    TexCoord = aTex;

    // map aPos (-1..1) -> centered pixel offset [-0.5*size .. +0.5*size]
    vec2 offset = aPos * 0.5 * scale; // offset in pixels relative to center

    // rotate around center
    float c = cos(rotation);
    float s = sin(rotation);
    mat2 rot = mat2(c, -s, s, c);
    vec2 rotated = rot * offset;

    // world position = center + rotated offset
    vec2 world = position + rotated;

    gl_Position = projection * vec4(world, 0.0, 1.0);
}
)";

const char* videoFragment = R"(
#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D videoTex;

void main() {
    vec4 color = texture(videoTex, TexCoord);
    FragColor = vec4(color.rgb, 1.0); // força alpha visível
}
)";

const char* textVertexShader = R"(
#version 330 core
layout (location = 0) in vec4 vertex;
out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 model;
uniform float rotation;

void main() {
    TexCoords = vertex.zw;

    float c = cos(rotation);
    float s = sin(rotation);
    mat2 rot = mat2(c, -s,
                    s,  c);

    vec2 rotatedPos = rot * vertex.xy;

    gl_Position = projection * model * vec4(rotatedPos, 0.0, 1.0);
}
)";

const char* textFragmentShader = R"(
#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D text;
uniform vec4 color;

void main() {
    float alpha = texture(text, TexCoords).r; // FreeType atlas stores grayscale in RED channel
    FragColor = vec4(color.rgb, color.a * alpha);
}
)";

const char* depth2DskinnedVertex = R"(
#version 420 core

layout(location = 0) in vec3  aPos;
layout(location = 3) in vec2  aTexCoord;
layout(location = 4) in uvec4 aJoint;
layout(location = 5) in vec4  aWeight;

out vec2 vTexCoord;

layout(std140, binding = 0) uniform Skin { mat4 bones[100]; };

uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main() {
    vTexCoord = aTexCoord;
    mat4 skinMat =
        aWeight.x * bones[aJoint.x]
        + aWeight.y * bones[aJoint.y]
        + aWeight.z * bones[aJoint.z]
        + aWeight.w * bones[aJoint.w];
    vec4 skinnedPos = skinMat * vec4(aPos, 1.0);
    gl_Position = lightSpaceMatrix * model * skinnedPos;
}
)";

const char* depth2DstaticVertex = R"(
#version 420 core

layout(location = 0) in vec3 aPos;
layout(location = 3) in vec2 aTexCoord;

out vec2 vTexCoord;

uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main() {
	vTexCoord = aTexCoord;
	gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}
)";

const char* depthCubeSkinnedVertex = R"(
#version 420 core

layout(location = 0) in vec3  aPos;
layout(location = 3) in vec2  aTexCoord;
layout(location = 4) in uvec4 aJoint;
layout(location = 5) in vec4  aWeight;

out vec2 vTexCoord;

layout(std140, binding = 0) uniform Skin { mat4 bones[100]; };

uniform mat4 model;
uniform mat4 shadowMatrices[15];

void main() {
    vTexCoord = aTexCoord;
    mat4 skinMat =
        aWeight.x * bones[aJoint.x]
        + aWeight.y * bones[aJoint.y]
        + aWeight.z * bones[aJoint.z]
        + aWeight.w * bones[aJoint.w];
    vec4 skinnedPos = skinMat * vec4(aPos, 1.0);
    vec4 worldPos = model * skinnedPos;
    gl_Position = shadowMatrices[gl_InstanceID] * worldPos;
}
)";

const char* depthCubeStaticVertex = R"(
#version 420 core

layout(location = 0) in vec3 aPos;
layout(location = 3) in vec2 aTexCoord;

out vec2 vTexCoord;

uniform mat4 model;
uniform mat4 shadowMatrices[15];

void main() {
	vTexCoord = aTexCoord;
	vec4 worldPos = model * vec4(aPos, 1.0);
	gl_Position = shadowMatrices[gl_InstanceID] * worldPos;
}
)";

const char* depth_fragment = R"(
#version 420 core

in vec2 vTexCoord;

uniform sampler2D baseColorTex;
uniform int  alphaMode;        // 0=OPAQUE, 1=MASK, 2=BLEND
uniform float alphaCutoff;
uniform float transmissionFactor;

void main() {
    // OPAQUE: nothing to do — depth is written automatically
    if (alphaMode == 0) return;

    float a = texture(baseColorTex, vTexCoord).a;

    // MASK (e.g. foliage, grilles): hard cutout — same silhouette as the color pass
    if (alphaMode == 1) {
        if (a < alphaCutoff) discard;
        return;
    }

    // BLEND / translucent: stochastic (dithered) shadow
    // Pixels are randomly discarded with probability proportional to transparency,
    // so a 50%-alpha surface casts ~50% of a full shadow on average.
    // transmissionFactor further reduces opacity (glass, water).
    float effectiveAlpha = a * (1.0 - transmissionFactor);
    float rnd = fract(sin(dot(gl_FragCoord.xy, vec2(12.9898, 78.233))) * 43758.5453);
    if (rnd > effectiveAlpha) discard;
}
)";

// For FiscionX High Quality Render Pipeline
const char* fragment = R"(
#version 330 core
const int LIGHT_DIRECTIONAL = 0;
const int LIGHT_POINT = 1;
const int LIGHT_SPOT = 2;
const float PI = 3.14159265359;
const int PCF_SAMPLES = 8;
const vec3 gridSamplingOffset[PCF_SAMPLES] = vec3[](
    vec3( 0.1,  0.1,  0.0),
    vec3(-0.1,  0.1,  0.0),
    vec3( 0.1, -0.1,  0.0),
    vec3(-0.1, -0.1,  0.0),
    vec3( 0.15, 0.0,  0.15),
    vec3(-0.15, 0.0,  0.15),
    vec3( 0.15, 0.0, -0.15),
    vec3(-0.15, 0.0, -0.15)
);
out vec4 FragColor;
layout(location = 1) out vec4 NormalRough;
layout(location = 2) out float MetallicOut;
in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec3 Tangent;
    vec3 Bitangent;
    vec2 TexCoords;
    vec4 FragPosLightSpace[15];
} fs_in;
in vec3 FragViewPos;
uniform float reflectionsStrength = 0.5;
uniform sampler2D baseColorTex;
uniform sampler2D normalMapTex;
uniform sampler2D glossinessTex;
uniform sampler2D specularF0Tex;
uniform int hasGlossinessMap;
uniform int hasSpecularF0Map;
uniform int useMetalRoughness;
uniform sampler2D metallicTex;
uniform float metallicFactor;
uniform float roughnessFactor;
uniform sampler2D shadowMaps[15];
uniform samplerCube shadowCubeMaps[15];
uniform sampler2D transmissionTex;
uniform sampler2DArray shadowMapDir;
uniform float cascadePlaneDistances[16];
uniform int cascadeCount;
uniform mat4 cascadeLightSpaceMatrices[16];
uniform mat4 view;
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D   brdfLUT;
uniform int         hasIBL;
uniform float transmissionFactor;
uniform int hasNormalMap;
uniform int numLights;
uniform sampler2D aoTex;
uniform int hasAOMap;
uniform int lightType[15];
uniform vec3 lightPos[15];
uniform vec3 lightDir[15];
uniform vec3 lightColor[15];
uniform float lightIntensity[15];
uniform float lightMaxDistance[15];
uniform float lightCutOff[15];
uniform float lightOuterCutOff[15];
uniform float lightConstant[15];
uniform float lightLinear[15];
uniform float lightQuadratic[15];
uniform vec3 viewPos;
uniform int alphaMode;
uniform float alphaCutoff;
uniform float environmentStrength;
uniform vec3 environmentSkyColor;
uniform vec3 environmentGroundColor;
uniform int isAffectedByLight;
uniform int acceptsShadows;
uniform float hdrExposure = 1.0;
uniform vec3 fogColor;
uniform float fogDensity;
uniform float fogStart;
uniform float fogEnd;
uniform int fogType;
const float diskRadius = 0.1;
vec4 baseSample = vec4(1.0);
const vec2 poisson16[16] = vec2[](
    vec2(-0.94201624, -0.39906216), vec2(0.94558609, -0.76890725),
    vec2(-0.094184101, -0.92938870), vec2(0.34495938, 0.29387760),
    vec2(-0.91588581, 0.45771432), vec2(-0.81544232, -0.87912464),
    vec2(-0.38277543, 0.27676845), vec2(0.97484398, 0.75648379),
    vec2(0.44323325, -0.97511554), vec2(0.53742981, -0.47373420),
    vec2(-0.26496911, -0.41893023), vec2(0.79197514, 0.19090188),
    vec2(-0.24188840, 0.99706507), vec2(-0.81409955, 0.91437590),
    vec2(0.19984126, 0.78641367), vec2(0.14383161, -0.14100790)
);
float interleavedGradientNoise(vec2 n) {
    return fract(3.378 * fract(dot(n, vec2(0.754877666, 0.56984029))));
}
vec3 ShadowTintColor(vec3 lightCol, float shadowAmt) {
    vec3 ambientAvg = (environmentSkyColor + environmentGroundColor) * 0.5;
    vec3 complementary = vec3(1.0) - lightCol;
    vec3 scatterColor = mix(ambientAvg, complementary, 0.35);
    scatterColor = max(scatterColor, vec3(0.02));
    float occlusion = clamp(shadowAmt, 0.0, 1.0);
    float scatterLum = dot(scatterColor, vec3(0.299, 0.587, 0.114));
    float darken = mix(1.0, 0.08 + 0.4 * scatterLum, occlusion);
    return mix(vec3(1.0), scatterColor * darken, occlusion);
}
float SampleCascadeShadow(int layer, vec3 fragPosWorld, vec3 N, vec3 L) {
    vec4 fragPosLS = cascadeLightSpaceMatrices[layer] * vec4(fragPosWorld, 1.0);
    vec3 projCoords = fragPosLS.xyz / fragPosLS.w;
    projCoords = projCoords * 0.5 + 0.5;
    float currentDepth = projCoords.z;
    if (currentDepth > 1.0) return 0.0;
    float layerScale = 1.0 + float(layer) * 1.5;
    float bias = max(0.0007 * (1.0 - dot(N, L)), 0.00007) * layerScale;
    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(shadowMapDir, 0));
    float noise = interleavedGradientNoise(gl_FragCoord.xy);
    float angle = noise * 2.0 * PI;
    float s = sin(angle);
    float c = cos(angle);
    mat2 rotation = mat2(c, -s, s, c);
    float spread = 1.5;
    for (int i = 0; i < 16; ++i) {
        vec2 offset = (rotation * poisson16[i]) * texelSize * spread;
        float pcfDepth = texture(shadowMapDir, vec3(projCoords.xy + offset, layer)).r;
        shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;
    }
    return shadow / 16.0;
}
float ShadowCalculationCSM(vec3 fragPosWorld, vec3 N, vec3 L) {
    if (acceptsShadows == 0) return 0.0;
    vec4 fragPosViewSpace = view * vec4(fragPosWorld, 1.0);
    float depthValue = abs(fragPosViewSpace.z);
    int layer = -1;
    for (int i = 0; i < cascadeCount; ++i) {
        if (depthValue < cascadePlaneDistances[i]) {
            layer = i;
            break;
        }
    }
    if (layer == -1) layer = max(0, cascadeCount - 1);
    float shadow = SampleCascadeShadow(layer, fragPosWorld, N, L);
    if (layer < cascadeCount - 1) {
        float splitDist = cascadePlaneDistances[layer];
        float blendRange = splitDist * 0.1;
        float distToSplit = splitDist - depthValue;
        if (distToSplit < blendRange) {
            float nextShadow = SampleCascadeShadow(layer + 1, fragPosWorld, N, L);
            float t = clamp(1.0 - (distToSplit / blendRange), 0.0, 1.0);
            shadow = mix(shadow, nextShadow, t);
        }
    }
    return shadow;
}
float ShadowCalculation2D(vec4 fragPosLS, sampler2D shadowMap, vec3 N, vec3 L) {
    if (acceptsShadows == 0) return 0.0;
    vec3 projCoords = fragPosLS.xyz / fragPosLS.w;
    projCoords = projCoords * 0.5 + 0.5;
    if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.y < 0.0 || projCoords.x > 1.0 || projCoords.y > 1.0)
        return 0.0;
    float currentDepth = projCoords.z;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    float bias = max(0.003 * (1.0 - dot(N, L)), 0.001);
    float noise = interleavedGradientNoise(gl_FragCoord.xy);
    float angle = noise * 2.0 * PI;
    float s = sin(angle);
    float c = cos(angle);
    mat2 rotation = mat2(c, -s, s, c);
    float shadow = 0.0;
    float spread = 1.8;
    for (int i = 0; i < 16; ++i) {
        vec2 offset = (rotation * poisson16[i]) * texelSize * spread;
        float closestDepth = texture(shadowMap, projCoords.xy + offset).r;
        if (currentDepth - bias > closestDepth) shadow += 1.0;
    }
    return shadow / 16.0;
}
float ShadowCalculationPoint(int idx, vec3 fragPos) {
    if (acceptsShadows == 0) return 0.0;
    vec3 fragToLight = fragPos - lightPos[idx];
    float currentDepth = length(fragToLight);
    float bias = 0.15;
    float shadow = 0.0;
    int samples = 16;
    float viewDistance = length(viewPos - fragPos);
    float diskRadiusLocal = (1.0 + (viewDistance / lightMaxDistance[idx])) * diskRadius;
    for (int i = 0; i < samples; ++i) {
        vec3 samplePos = fragToLight + gridSamplingOffset[i % 8] * diskRadiusLocal;
        float closestDepth = texture(shadowCubeMaps[idx], samplePos).r;
        closestDepth *= lightMaxDistance[idx];
        if (currentDepth - bias > closestDepth) shadow += 1.0;
    }
    return shadow / float(samples);
}
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    vec3 fresnel = F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
    return mix(F0, fresnel, reflectionsStrength);
}
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return a2 / max(denom, 0.0000001);
}
float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    float denom = NdotV * (1.0 - k) + k;
    return NdotV / max(denom, 0.0000001);
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}
void main() {
    baseSample = texture(baseColorTex, fs_in.TexCoords);
    if (alphaMode == 1 && baseSample.a < alphaCutoff) discard;
    vec3 baseColor = baseSample.rgb;
    float metallic;
    float roughness;
    vec3  F0_base;
    if (useMetalRoughness == 1) {
        vec4 metallicProps = texture(metallicTex, fs_in.TexCoords);
        roughness = metallicProps.g * roughnessFactor;
        metallic = metallicProps.b * metallicFactor;
        F0_base = mix(vec3(0.04), baseColor, metallic);
    }
    else if (hasGlossinessMap == 1) {
        float glossiness = texture(glossinessTex, fs_in.TexCoords).a;
        roughness = 1.0 - glossiness;
        metallic = 0.0;
        if (hasSpecularF0Map == 1) {
            F0_base = texture(specularF0Tex, fs_in.TexCoords).rgb;
        }
        else {
            F0_base = vec3(0.04);
        }
    }
    else {
        metallic = metallicFactor;
        roughness = roughnessFactor;
        F0_base = mix(vec3(0.04), baseColor, metallic);
    }
    roughness = clamp(roughness, 0.01, 1.0);
    vec3 N;
    if (hasNormalMap == 1) {
        vec2 rg = texture(normalMapTex, fs_in.TexCoords).rg * 2.0 - 1.0;
        vec3 tangentNormal = vec3(rg, sqrt(max(0.0, 1.0 - dot(rg, rg))));
        vec3 T = normalize(fs_in.Tangent);
        vec3 B = normalize(fs_in.Bitangent);
        vec3 Nor = normalize(fs_in.Normal);
        mat3 TBN = mat3(T, B, Nor);
        N = normalize(TBN * tangentNormal);
    }
    else {
        N = normalize(fs_in.Normal);
    }
    vec3 V = normalize(viewPos - fs_in.FragPos);
    
    if (!gl_FrontFacing) {
        N = -N;
    }
    vec3 ambientSum = vec3(0.0);
    vec3 directLightSum = vec3(0.0);
    vec3 ambientContribution;
    if (hasIBL == 1) {
        vec3 kS_amb = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0_base, roughness);
        vec3 kD_amb = (vec3(1.0) - kS_amb) * (1.0 - metallic);
        const float IBL_SAMPLE_MAX = 20.0;
        vec3 irradianceDir = N;
        vec3 reflectDirForSample = reflect(-V, N);
        vec3 irradiance = min(texture(irradianceMap, irradianceDir).rgb, vec3(IBL_SAMPLE_MAX));
        vec3 diffuse_ibl = irradiance * baseColor;
        const float MAX_REFLECTION_LOD = 4.0;
        vec3 R = reflectDirForSample;
        vec3 prefilteredColor = min(textureLod(prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb, vec3(IBL_SAMPLE_MAX));
        vec2 envBRDF = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
        vec3 specular_ibl = prefilteredColor * (F0_base * envBRDF.x + envBRDF.y) * reflectionsStrength;
        ambientContribution = (kD_amb * diffuse_ibl + specular_ibl) * environmentStrength;
    }
    else {
        float hemi = 0.5 * (dot(N, vec3(0, 1, 0)) + 1.0);
        vec3 ambientHemi = mix(environmentGroundColor, environmentSkyColor, hemi);
        ambientContribution = baseColor * ambientHemi * environmentStrength;
    }
    const float IBL_MAX = 10.0;
    ambientContribution = min(ambientContribution, vec3(IBL_MAX));
    float ao = 1.0;
    if (hasAOMap == 1) {
        ao = texture(aoTex, fs_in.TexCoords).r;
    }
    ambientContribution *= ao;
    const float localAmbientFactor = 0.05;
    for (int i = 0; i < 15; ++i) {
        if (i >= numLights) break;
        int ltype = lightType[i];
        vec3 L;
        float attenuation = 1.0;
        float intensity = lightIntensity[i];
        float shadow = 0.0;
        vec3 lightCol = lightColor[i];
        if (ltype == LIGHT_DIRECTIONAL) {
            L = normalize(-lightDir[i]);
            float NdotL = dot(N, L);
            if (NdotL <= 0.0) {
                ambientSum += localAmbientFactor * baseColor * lightCol * intensity;
                continue;
            }
            shadow = ShadowCalculationCSM(fs_in.FragPos, N, L);
        }
        else if (ltype == LIGHT_POINT) {
            vec3 toLight = lightPos[i] - fs_in.FragPos;
            float dist = length(toLight);
            if (dist > lightMaxDistance[i]) continue;
            L = normalize(toLight);
            float attFactor = clamp(1.0 - dist / lightMaxDistance[i], 0.0, 1.0);
            attenuation = attFactor * attFactor;
            float NdotL = dot(N, L);
            if (NdotL <= 0.0) {
                ambientSum += attenuation * localAmbientFactor * baseColor * lightCol * intensity;
                continue;
            }
            shadow = ShadowCalculationPoint(i, fs_in.FragPos);
        }
        else {
            vec3 toLight = lightPos[i] - fs_in.FragPos;
            float dist = length(toLight);
            if (dist > lightMaxDistance[i]) continue;
            L = normalize(toLight);
            attenuation = 1.0 / (lightConstant[i] + lightLinear[i] * dist + lightQuadratic[i] * dist * dist);
            float theta = dot(-L, normalize(lightDir[i]));
            float eps = max(lightCutOff[i] - lightOuterCutOff[i], 0.001);
            float spotIntensity = clamp((theta - lightOuterCutOff[i]) / eps, 0.0, 1.0);
            intensity *= spotIntensity;
            float NdotL = dot(N, L);
            if (NdotL <= 0.0) {
                ambientSum += attenuation * localAmbientFactor * baseColor * lightCol * intensity;
                continue;
            }
            shadow = ShadowCalculation2D(fs_in.FragPosLightSpace[i], shadowMaps[i], N, L);
        }
        vec3 H = normalize(L + V);
        float NdotL = max(dot(N, L), 0.0);
        float VdotH = max(dot(V, H), 0.0);
        vec3 F = fresnelSchlick(VdotH, F0_base);
        float D = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 specularBRDF = (D * G * F) / max(4.0 * NdotL * max(dot(N, V), 0.0), 0.000001);
        specularBRDF *= reflectionsStrength;
        vec3 kS = F;
        vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);
        vec3 diffuse = kD * baseColor / PI;
        vec3 directContrib = (diffuse + specularBRDF) * lightCol * NdotL * intensity;
        vec3 shadowColor = ShadowTintColor(lightCol, shadow);
        directLightSum += attenuation * directContrib * shadowColor;
        ambientSum += attenuation * localAmbientFactor * baseColor * lightCol * intensity;
    }
    float transAmt = texture(transmissionTex, fs_in.TexCoords).r * transmissionFactor;
    vec3 transLight = baseColor * transAmt * 0.5;
    vec3 result = ambientContribution + ambientSum + directLightSum;
    result = mix(result, result + transLight, transAmt);
#define ACES(c) clamp(((c)*(2.51*(c)+0.03))/((c)*(2.43*(c)+0.59)+0.14),0.0,1.0)
#define TO_SRGB(c) pow(max((c),vec3(0.0)),vec3(1.0/2.2))
    float distance = length(FragViewPos);
    float fogFactor = 1.0;
    if (fogType == 0) {
        fogFactor = (fogEnd - distance) / (fogEnd - fogStart);
    }
    else if (fogType == 1) {
        fogFactor = exp(-fogDensity * distance);
    }
    else if (fogType == 2) {
        fogFactor = exp(-pow(fogDensity * distance, 2.0));
    }
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    vec4 finalPixelColour;
    if (isAffectedByLight == 0) {
        vec3 fallback = ambientContribution + ambientSum;
        fallback = max(fallback, baseColor * 0.7);
        fallback = mix(fallback, fallback + transLight, transAmt);
        finalPixelColour = vec4(TO_SRGB(ACES(fallback * hdrExposure)), (alphaMode == 2 ? baseSample.a : 1.0));
    }
    else {
        finalPixelColour = vec4(TO_SRGB(ACES(result * hdrExposure)), (alphaMode == 2 ? baseSample.a : 1.0));
    }
    FragColor = vec4(mix(fogColor, finalPixelColour.rgb, fogFactor), finalPixelColour.a);
#undef ACES
#undef TO_SRGB
    if (alphaMode == 2) {
        NormalRough = vec4(0.0, 0.0, 0.0, -1.0);
        MetallicOut = 0.0;
    }
    else {
        vec3 viewSpaceN = normalize(mat3(view) * N);
        NormalRough = vec4(viewSpaceN, roughness);
        MetallicOut = metallic;
    }
}
)";

const char* imageFragment = R"(
#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D tex;
uniform float alpha;

void main() {
    vec4 color = texture(tex, TexCoord);
    float finalAlpha = color.a * alpha;
    if (finalAlpha < 0.01) discard; // pixels transparentes nao forcam depth=1.0
    FragColor = vec4(color.rgb, finalAlpha);
}
)";

const char* imageVertex = R"(
#version 330 core

layout (location = 0) in vec2 aPos;   // quad definido em [0,1] (não [-1,1]!)
layout (location = 1) in vec2 aTex;

out vec2 TexCoord;

uniform vec2 position;
uniform vec2 scale;
uniform float rotation;
uniform mat4 projection;
uniform float width;
uniform float height;

void main()
{
    vec2 centered = (aPos - 0.5) * vec2(width, height) * scale;

    mat2 rot = mat2(cos(rotation), -sin(rotation),
                    sin(rotation),  cos(rotation));
    vec2 rotated = rot * centered;

    vec2 translated = rotated + position;

    gl_Position = projection * vec4(translated, 0.0, 1.0);

    TexCoord = aTex;
}
)";

const char* image3DFragment = R"(
#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D tex;
uniform float alpha;

void main() {
    vec4 color = texture(tex, TexCoord);
    float finalAlpha = color.a * alpha;
    if (finalAlpha < 0.01) discard; // pixels transparentes nao escrevem profundidade/cor
    FragColor = vec4(color.rgb, finalAlpha);
}
)";

const char* image3DVertex = R"(
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

out vec2 TexCoord;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    TexCoord = aTex;
}
)";

const char* vertexSkinned = R"(
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec4 aTangent;
layout(location = 3) in vec2 aTexCoord;
layout(location = 4) in uvec4 aJoint;
layout(location = 5) in vec4 aWeight;

layout(std140) uniform Skin {
    mat4 bones[100];
};

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec3 Tangent;
    vec3 Bitangent;
    vec2 TexCoords;
    vec4 FragPosLightSpace[15];
} vs_out;

out vec3 FragViewPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrices[15];
// OPTIM: normalMatrix precomputed on CPU — avoids transpose(inverse(model)) per vertex
uniform mat3 normalMatrix;

vec4 skinPosition(vec4 pos) {
    mat4 skinMat =
          aWeight.x * bones[aJoint.x]
        + aWeight.y * bones[aJoint.y]
        + aWeight.z * bones[aJoint.z]
        + aWeight.w * bones[aJoint.w];
    return skinMat * pos;
}

void main() {
    vec4 viewPos = view * model * vec4(aPos, 1.0);
    FragViewPos = viewPos.xyz;

    vec4 skinned = skinPosition(vec4(aPos, 1.0));
    vec4 worldPos = model * skinned;
    vs_out.FragPos = worldPos.xyz;

    vs_out.Normal = normalize(normalMatrix * aNormal);

    vec3 T = normalize(mat3(model) * aTangent.xyz);
    vec3 N = normalize(vs_out.Normal);
    vec3 B = cross(N, T) * aTangent.w;
    vs_out.Tangent = T;
    vs_out.Bitangent = B;

    vs_out.TexCoords = aTexCoord;

    for (int i = 0; i < 15; ++i) {
        vs_out.FragPosLightSpace[i] = lightSpaceMatrices[i] * worldPos;
    }

    gl_Position = projection * view * worldPos;
}
)";

const char* vertexStatic = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec4 aTangent;
layout(location = 3) in vec2 aTexCoord;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec3 Tangent;
    vec3 Bitangent;
    vec2 TexCoords;
    vec4 FragPosLightSpace[15];
} vs_out;

out vec3 FragViewPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrices[15]; // um por luz
// OPTIM: normalMatrix precomputed on CPU — avoids transpose(inverse(model)) per vertex
uniform mat3 normalMatrix;

void main() {
    vec4 viewPos = view * model * vec4(aPos, 1.0);
    FragViewPos = viewPos.xyz;

    vec4 worldPos = model * vec4(aPos, 1.0);
    vs_out.FragPos = worldPos.xyz;

    // OPTIM: use precomputed normalMatrix uniform instead of per-vertex inverse
    vs_out.Normal = normalize(normalMatrix * aNormal);

    vec3 T = normalize(mat3(model) * aTangent.xyz);
    vec3 N = normalize(vs_out.Normal);
    vec3 B = cross(N, T) * aTangent.w;
    vs_out.Tangent = T;
    vs_out.Bitangent = B;

    vs_out.TexCoords = aTexCoord;

    for (int i = 0; i < 15; ++i) {
        vs_out.FragPosLightSpace[i] = lightSpaceMatrices[i] * worldPos;
    }

    gl_Position = projection * view * worldPos;
}
)";

const char* postProcessVertex = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main() {
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}
)";

const char* godRaysFragment = R"(
#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D depthTexture;
uniform sampler2D ssaoTexture;

uniform vec2 lightPosOnScreen;
uniform float sunVisibility;
uniform float aspect;
uniform float time;

uniform float sunDiskSize;
uniform float sunHaloSize;
uniform vec3 sunColor;

uniform vec3 colorCorrection = vec3(0.07, 0.07, 0.07);

uniform float rayDensity;
uniform float rayWeight;
uniform float rayDecay;
uniform float rayExposure;
uniform int NUM_SAMPLES;

uniform float nearPlane;
uniform float farPlane;

// ------------------ HELPER FUNCTIONS ------------------
float hash(vec2 p) {
    p = fract(p * vec2(123.34, 456.21));
    p += dot(p, p + 45.32);
    return fract(p.x * p.y);
}

float LinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0;
    return (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));
}

float GetSunVisibility(vec2 sunPos) {
    float visibleSamples = 0.0;
    float worldSunDistance = farPlane * 0.98;
    const float range = 0.012; 
    for(float x = -1.0; x <= 1.0; x += 1.0) {
        for(float y = -1.0; y <= 1.0; y += 1.0) {
            vec2 offset = vec2(x, y) * range;
            float d = texture(depthTexture, sunPos + offset).r;
            if (LinearizeDepth(d) >= worldSunDistance) visibleSamples += 1.0;
        }
    }
    return (visibleSamples / 9.0);
}

// Desenha um "fantasma" com aberração cromática opcional
vec3 drawGhost(vec2 uv, vec2 pos, float size, float falloff, vec3 color, float chromAb) {
    vec2 dir = normalize(pos - uv);
    float r = smoothstep(size, size * falloff, distance(uv + dir * chromAb, pos));
    float g = smoothstep(size, size * falloff, distance(uv, pos));
    float b = smoothstep(size, size * falloff, distance(uv - dir * chromAb, pos));
    return vec3(r, g, b) * color;
}

void main() {
    vec3 sceneColor = texture(screenTexture, TexCoords).rgb;
    vec4 ssaoSample = texture(ssaoTexture, TexCoords);
    sceneColor = sceneColor * ssaoSample.a + ssaoSample.rgb;
    vec2 uv_corr = TexCoords * vec2(aspect, 1.0);
    vec2 sunPos_corr = lightPosOnScreen * vec2(aspect, 1.0);
    vec2 center_corr = vec2(0.5) * vec2(aspect, 1.0);
    vec2 sunVec = center_corr - sunPos_corr;

    float softVisibility = GetSunVisibility(lightPosOnScreen) * sunVisibility;
    float worldSunDistance = farPlane * 0.98;
    float localOcclusion = step(LinearizeDepth(texture(depthTexture, TexCoords).r), worldSunDistance - 1.0);

    if (softVisibility <= 0.001 && localOcclusion > 0.5) {
        FragColor = vec4(sceneColor - colorCorrection, 1.0);
        return;
    }

    // 1. PROCEDURAL SUN & RAYS
    float distToSun = distance(uv_corr, sunPos_corr);
    float sunDisk = smoothstep(sunDiskSize, sunDiskSize * 0.8, distToSun);
    float sunHalo = pow(smoothstep(sunHaloSize, 0.0, distToSun), 4.0);
    vec3 proceduralSun = sunColor * (sunDisk * 45.0 + sunHalo * 3.0) * (1.0 - localOcclusion) * sunVisibility;

    vec2 deltaTexCoord = (TexCoords - lightPosOnScreen) * (1.0 / float(NUM_SAMPLES)) * rayDensity;
    vec2 coord = TexCoords + deltaTexCoord * hash(TexCoords + time);
    vec3 raysColor = vec3(0.0);
    float illuminationDecay = 1.0;

    for (int i = 0; i < NUM_SAMPLES; i++) {
        coord -= deltaTexCoord;
        float mask = pow(smoothstep(0.25, 0.0, distance(coord * vec2(aspect, 1.0), sunPos_corr)), 3.0);
        float occ = step(LinearizeDepth(texture(depthTexture, coord).r), worldSunDistance - 1.0);
        raysColor += sunColor * mask * illuminationDecay * rayWeight * (1.0 - occ);
        illuminationDecay *= rayDecay;
    }

    // 2. LENS FLARE EVOLUÍDO
    vec3 flareFinal = vec3(0.0);

    // Grande Glow Suave de Lente
    flareFinal += vec3(0.1, 0.15, 0.2) * exp(-distToSun * 1.5) * 0.2;

    // Fantasmas em linha (Distribuídos ao longo do sunVec)
    // Coordenadas baseadas no centro para inverter os reflexos
    flareFinal += drawGhost(uv_corr, center_corr + sunVec * 0.4,  0.06, 0.0, vec3(0.15, 0.12, 0.1), 0.005);
    flareFinal += drawGhost(uv_corr, center_corr + sunVec * 0.7,  0.03, 0.3, vec3(0.1, 0.2, 0.1), 0.008);
    flareFinal += drawGhost(uv_corr, center_corr + sunVec * -0.3, 0.02, 0.1, vec3(0.2, 0.1, 0.1), 0.01);
    flareFinal += drawGhost(uv_corr, center_corr + sunVec * -0.6, 0.09, 0.8, vec3(0.05, 0.05, 0.1), 0.0);
    flareFinal += drawGhost(uv_corr, center_corr + sunVec * 1.2,  0.15, 0.9, vec3(0.02, 0.02, 0.05), 0.0);

    // Anéis (Ring Elements)
    float ring1 = distance(uv_corr, center_corr + sunVec * 0.6);
    flareFinal += smoothstep(0.01, 0.0, abs(ring1 - 0.25)) * vec3(0.1, 0.08, 0.05) * 0.4;
    
    float ring2 = distance(uv_corr, center_corr + sunVec * 0.2);
    flareFinal += smoothstep(0.04, 0.0, abs(ring2 - 0.5)) * vec3(0.05, 0.05, 0.1) * 0.2;

    // Starburst (Pequenos raios de difração fixos)
    float ang = atan(uv_corr.y - sunPos_corr.y,
                 uv_corr.x - sunPos_corr.x);

    float ray1 = pow(max(0.0, 0.5 + 0.5 * sin(ang * 6.0 + 1.5)), 10.0);
    float ray2 = pow(max(0.0, 0.5 + 0.5 * sin(ang * 10.0)), 10.0);

    float star = ray1 * 0.5 + ray2 * 0.3;

    flareFinal += star * sunColor * exp(-distToSun * 25.0) * 2.0;

    // 3. FINALIZAÇÃO
    // Vinheta na borda para o flare não "cortar" bruscamente nas beiradas da tela
    float edgeFade = smoothstep(1.2, 0.5, length(TexCoords - 0.5));
    flareFinal *= edgeFade;

    vec3 finalEffect = (proceduralSun * 0.3) + 
                       (raysColor * rayExposure * softVisibility) + 
                       (flareFinal * softVisibility);

    // Tonemapping
    vec3 effectLDR = vec3(1.0) - exp(-finalEffect * 1.2);
    vec3 color = sceneColor + effectLDR;

    FragColor = vec4(clamp(color, 0.0, 1.0) - colorCorrection, 1.0);
}
)";

const char* ssaoFragment = R"(
#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D depthTexture;
uniform sampler2D colorTexture;
uniform sampler2D noiseTex;

uniform mat4 projection;
uniform mat4 invProjection;

uniform vec2 screenSize;
uniform float nearPlane;
uniform float farPlane;

uniform float radius;
uniform float bias;
uniform float intensity;
uniform float giStrength;

const int KERNEL_SIZE = 16;
uniform vec3 samples[KERNEL_SIZE];

float LinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0;
    return (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));
}

vec3 ViewPosFromDepth(vec2 uv, float depth) {
    vec4 clip = vec4(uv * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    vec4 view = invProjection * clip;
    return view.xyz / view.w;
}

vec3 ViewNormalFromDepth(vec2 uv, vec3 viewPos) {
    vec2 texel = 1.0 / screenSize;

    float dC = texture(depthTexture, uv).r;
    float dR = texture(depthTexture, uv + vec2(texel.x, 0.0)).r;
    float dL = texture(depthTexture, uv - vec2(texel.x, 0.0)).r;
    float dU = texture(depthTexture, uv + vec2(0.0, texel.y)).r;
    float dD = texture(depthTexture, uv - vec2(0.0, texel.y)).r;

    vec2 hUV = (abs(dR - dC) < abs(dL - dC)) ? uv + vec2(texel.x, 0.0) : uv - vec2(texel.x, 0.0);
    vec2 vUV = (abs(dU - dC) < abs(dD - dC)) ? uv + vec2(0.0, texel.y) : uv - vec2(0.0, texel.y);

    float dH = texture(depthTexture, hUV).r;
    float dV = texture(depthTexture, vUV).r;

    vec3 posH = ViewPosFromDepth(hUV, dH);
    vec3 posV = ViewPosFromDepth(vUV, dV);

    vec3 dx = posH - viewPos;
    vec3 dy = posV - viewPos;

    vec3 n = cross(dx, dy);
    if ((hUV.x - uv.x) < 0.0) n = -n;
    if ((vUV.y - uv.y) < 0.0) n = -n;

    if (dot(n, n) < 1e-12) return vec3(0.0, 0.0, 1.0);
    return normalize(n);
}

void main() {
    float depth = texture(depthTexture, TexCoords).r;
    if (depth >= 0.9999) {
        // Sem geometria opaca aqui (céu, ou objeto 3D transparente / overlay 2D
        // que não escreve no depth buffer). Não aplicar AO nem GI:
        // occlusion = 1.0 (alpha=1 -> sceneColor * 1.0) e indirect = 0.0.
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    vec3 fragPos = ViewPosFromDepth(TexCoords, depth);
    vec3 normal = ViewNormalFromDepth(TexCoords, fragPos);

    vec2 noiseScale = screenSize / 4.0;
    vec3 randomVec = normalize(texture(noiseTex, TexCoords * noiseScale).xyz * 2.0 - 1.0);

    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    vec3 indirect = vec3(0.0);

    for (int i = 0; i < KERNEL_SIZE; i++) {
        vec3 samplePos = TBN * samples[i];
        samplePos = fragPos + samplePos * radius;

        vec4 offset = projection * vec4(samplePos, 1.0);
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;

        if (offset.x < 0.0 || offset.x > 1.0 || offset.y < 0.0 || offset.y > 1.0) continue;

        vec4 occluderSample = texture(colorTexture, offset.xy);

        float sampleDepth = texture(depthTexture, offset.xy).r;
        if (sampleDepth >= 0.9999) continue;
        vec3 occluderViewPos = ViewPosFromDepth(offset.xy, sampleDepth);

        float rangeCheck = smoothstep(0.0, 1.0, radius / max(abs(fragPos.z - occluderViewPos.z), 0.0001));

        if (occluderViewPos.z >= samplePos.z + bias) {
            occlusion += rangeCheck;

            vec3 occluderNormal = ViewNormalFromDepth(offset.xy, occluderViewPos);
            vec3 toFrag = normalize(fragPos - occluderViewPos);
            float bounceFactor = max(dot(occluderNormal, toFrag), 0.0);

            indirect += occluderSample.rgb * bounceFactor * rangeCheck;
        }
    }

    occlusion = 1.0 - (occlusion / float(KERNEL_SIZE));
    occlusion = clamp(pow(occlusion, intensity), 0.0, 1.0);

    indirect = (indirect / float(KERNEL_SIZE)) * giStrength;

    FragColor = vec4(indirect, occlusion);
}
)";

const char* ssaoBlurFragment = R"(
#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D ssaoInput;
uniform vec2 screenSize;

void main() {
    vec2 texel = 1.0 / screenSize;
    vec4 result = vec4(0.0);
    for (int x = -2; x <= 2; x++) {
        for (int y = -2; y <= 2; y++) {
            result += texture(ssaoInput, TexCoords + vec2(float(x), float(y)) * texel);
        }
    }
    FragColor = result / 25.0;
}
)";

// ============================================================
// SSR (Screen Space Reflections)
// ============================================================
// Ray march em view-space sobre o depth buffer da cena já renderizada (mainDepthBuffer),
// usando o "G-buffer-lite" (mainNormalRoughBuffer: normal view-space + roughness) escrito
// pelo fragment shader principal. Roda como passe de pós-processamento (mesmo padrão do
// SSAO): não há acoplamento com o shader de material, então funciona em cima do forward
// renderer existente sem reescrever o pipeline de shading.
//
// Saída: ssrColorBuffer.rgb = cor refletida (já amostrada de mainColorBuffer no ponto de
// hit), ssrColorBuffer.a = confiança do hit (0 = sem reflexo, o material mantém seu
// specular_ibl normal vindo do prefiltered cubemap; até 1 = hit sólido e confiável em tela).
// Esse resultado bruto passa por um blur roughness-aware (ssrBlurFragment, logo abaixo —
// raio do kernel escala com a roughness do pixel, simulando a dispersão de um reflexo glossy
// que o ray march em si, sendo um raio de espelho perfeito, não produz) antes de chegar ao
// composite. O composite (ssrCompositeFragment, mais abaixo) funde por interpolação —
// mix(sceneColor, ssrColor, blend) — e não por soma, então a contribuição de SSR substitui
// parte do specular já presente na cena em vez de se somar a ele (evitando reflexos
// "estourados"/duplicados).
const char* ssrFragment = R"(
#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D depthTexture;
uniform sampler2D colorTexture;
uniform sampler2D normalRoughTexture;

uniform mat4 projection;
uniform mat4 invProjection;

uniform vec2 screenSize;
uniform float nearPlane;
uniform float farPlane;

uniform float maxDistance;
uniform float thickness;
uniform int   maxSteps;
uniform int   binarySteps;
uniform float stride;
uniform float edgeFade;

vec3 ViewPosFromDepth(vec2 uv, float depth) {
    vec4 clip = vec4(uv * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    vec4 view = invProjection * clip;
    return view.xyz / view.w;
}

// Projeta um ponto em view-space para coordenadas de tela [0,1] + a profundidade não-linear
// ([0,1], igual ao que está armazenado no depth buffer) correspondente a esse ponto.
vec3 ViewToScreen(vec3 viewPos) {
    vec4 clip = projection * vec4(viewPos, 1.0);
    vec3 ndc = clip.xyz / clip.w;
    return vec3(ndc.xy * 0.5 + 0.5, ndc.z * 0.5 + 0.5);
}

void main() {
    float depth = texture(depthTexture, TexCoords).r;
    if (depth >= 0.9999) {
        // Céu / sem geometria opaca: não há o que refletir.
        FragColor = vec4(0.0, 0.0, 0.0, 0.0);
        return;
    }

    vec4 nr = texture(normalRoughTexture, TexCoords);
    if (nr.a < 0.0) {
        // Sentinela: pixel pertence a uma malha BLEND (não participa do SSR).
        FragColor = vec4(0.0, 0.0, 0.0, 0.0);
        return;
    }

    vec3 N = normalize(nr.rgb);
    float roughness = clamp(nr.a, 0.0, 1.0);

    vec3 viewPos = ViewPosFromDepth(TexCoords, depth);
    vec3 incident = normalize(viewPos); // câmera na origem em view-space: direção do olho até a superfície
    vec3 R = normalize(reflect(incident, N));

    // Superfícies extremamente rugosas (quase difusas) não rendem nada útil: o custo do march
    // não compensa. O fade suave por roughness fica a cargo do composite (ssrCompositeFragment),
    // que já reduz a contribuição gradualmente — aqui só cortamos o caso extremo para economizar
    // os passos do march em pixels que de qualquer forma terão blend ~0.
    if (roughness > 0.95) {
        FragColor = vec4(0.0, 0.0, 0.0, 0.0);
        return;
    }

    // viewPos.z é negativo (câmera olha para -Z) e cresce em módulo com a distância — usamos
    // |viewPos.z| para escalar o offset/stride/thickness com a distância da câmera. Um offset
    // fixo em metros (ex.: 0.02) é insignificante a 50m de distância (causa self-intersection
    // logo no 1º passo, gerando ruído/falsos-hits) e exagerado a 0.5m de distância. Escalar
    // por uma fração da profundidade resolve ambos os casos com os mesmos parâmetros.
    float viewDist = max(abs(viewPos.z), nearPlane);
    float originBias = max(viewDist * 0.01, 0.015);

    // Empurra a origem do raio para fora da superfície ao longo da normal, evitando
    // auto-interseção imediata (self-hit) no primeiro passo do march.
    vec3 rayOrigin = viewPos + N * originBias;

    // Step também escalado pela distância: perto da câmera os steps precisam ser pequenos
    // (mais precisão), longe podem ser maiores (mesmo alcance em menos iterações).
    float rayStride = max(stride * viewDist * 0.1, 0.02);
    int steps = max(maxSteps, 1);

    vec3 prevSamplePos = rayOrigin;
    vec3 hitColor = vec3(0.0);
    float hitConfidence = 0.0;
    vec2 hitUV = vec2(0.0);

    bool found = false;
    vec3 samplePos = rayOrigin;

    for (int i = 1; i <= steps; i++) {
        prevSamplePos = samplePos;
        samplePos = rayOrigin + R * (rayStride * float(i));

        float travelled = length(samplePos - rayOrigin);
        if (travelled > maxDistance) break;

        vec3 screenPos = ViewToScreen(samplePos);
        if (screenPos.x < 0.0 || screenPos.x > 1.0 || screenPos.y < 0.0 || screenPos.y > 1.0) break;
        if (screenPos.z < 0.0 || screenPos.z > 1.0) break;

        float sceneDepth = texture(depthTexture, screenPos.xy).r;
        if (sceneDepth >= 0.9999) continue; // céu nessa direção, sem hit possível aqui

        vec3 sceneViewPos = ViewPosFromDepth(screenPos.xy, sceneDepth);

        // Compara profundidade ao longo do eixo da câmera (view-space Z, negativo "para frente").
        float rayDepthVS = samplePos.z;
        float sceneDepthVS = sceneViewPos.z;

        // Tolerância de espessura escalada pela distância da cena nesse pixel: o depth buffer
        // não-linear perde precisão rapidamente longe da câmera, então uma tolerância fixa
        // (ex.: 0.4 sempre) ou é grossa demais perto (aceita hits errados/manchas na lataria)
        // ou fina demais longe (rejeita hits válidos, deixando superfícies distantes "apagadas").
        float thicknessScaled = max(thickness * max(abs(sceneDepthVS), nearPlane) * 0.05, thickness * 0.1);

        // O raio "passou por trás" da geometria: possível hit. A tolerância evita falsos
        // positivos atravessando objetos finos/distantes ao longo do raio.
        if (rayDepthVS <= sceneDepthVS) {
            float depthDelta = sceneDepthVS - rayDepthVS;
            if (depthDelta < thicknessScaled) {
                // Refinamento por busca binária entre prevSamplePos (na frente da superfície)
                // e samplePos (atrás dela), para convergir no ponto exato de interseção.
                vec3 lo = prevSamplePos;
                vec3 hi = samplePos;
                vec2 refinedUV = screenPos.xy;

                for (int b = 0; b < binarySteps; b++) {
                    vec3 mid = mix(lo, hi, 0.5);
                    vec3 midScreen = ViewToScreen(mid);
                    if (midScreen.x < 0.0 || midScreen.x > 1.0 || midScreen.y < 0.0 || midScreen.y > 1.0) {
                        hi = mid;
                        continue;
                    }
                    float midSceneDepth = texture(depthTexture, midScreen.xy).r;
                    if (midSceneDepth >= 0.9999) {
                        hi = mid;
                        continue;
                    }
                    vec3 midSceneViewPos = ViewPosFromDepth(midScreen.xy, midSceneDepth);
                    if (mid.z <= midSceneViewPos.z) {
                        hi = mid;
                        refinedUV = midScreen.xy;
                    } else {
                        lo = mid;
                    }
                }

                hitUV = refinedUV;
                hitColor = texture(colorTexture, hitUV).rgb;
                found = true;

                // Confiança: cai perto das bordas da tela e perto do limite de maxDistance,
                // para que a transição para o fallback de IBL (no shader de material) seja suave
                // em vez de um corte abrupto quando o raio sai do que está visível em tela.
                vec2 edgeDist = min(hitUV, 1.0 - hitUV);
                float screenEdgeFactor = smoothstep(0.0, edgeFade, min(edgeDist.x, edgeDist.y));

                float distFactor = 1.0 - smoothstep(maxDistance * 0.75, maxDistance, travelled);

                // Reflexos quase tangentes à tela (R quase perpendicular ao eixo da câmera)
                // sofrem de baixa precisão numérica no march (passos minúsculos em screen-space
                // por grande distância percorrida em view-space) — atenua só esse extremo.
                // A variação "visual" de intensidade por ângulo (Fresnel) fica a cargo do
                // composite, que é o lugar fisicamente correto para isso.
                float facingFactor = smoothstep(0.0, 0.15, abs(incident.z));

                hitConfidence = screenEdgeFactor * distFactor * facingFactor;
                break;
            }
        }
    }

    if (!found) {
        FragColor = vec4(0.0, 0.0, 0.0, 0.0);
        return;
    }

    FragColor = vec4(hitColor, clamp(hitConfidence, 0.0, 1.0));
}
)";

// Borra ssrColorBuffer com raio escalado pela roughness do pixel central (lida de
// mainNormalRoughBuffer), aproximando o espalhamento que um reflexo "glossy" real teria.
// O ray march em ssrFragment sempre traça um raio de espelho perfeito (sem dispersão), então
// sem este passe a única coisa que a roughness mudava era a OPACIDADE da reflexão no composite
// — a imagem refletida em si continuava perfeitamente nítida em qualquer roughness abaixo do
// início da janela de fade. Isso faz superfícies de roughness média (lataria suja, plástico,
// concreto polido) refletirem com nitidez de espelho em vez do brilho difuso esperado.
//
// Acumula em espaço premultiplied-alpha (rgb * a) — se fizéssemos a média direta de rgb,
// pixels vizinhos sem hit (a = 0, rgb indefinido/zero) vazariam preto pro resultado borrado
// mesmo perto da borda de um reflexo válido. Dividindo por alphaSum no final, só a cor dos
// vizinhos que de fato tinham reflexo entra na média.
const char* ssrBlurFragment = R"(
#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D ssrInput;           // ssrColorBuffer bruto (rgb = cor do hit, a = confiança)
uniform sampler2D normalRoughTexture; // só a roughness (canal a) é usada aqui
uniform vec2 screenSize;
uniform float maxBlurRadius;          // raio em pixels atingido quando roughness = 1.0; 0 desliga o blur

void main() {
    vec4 center = texture(ssrInput, TexCoords);

    // Sem hit no pixel central: nada a borrar, e não queremos que o blur "invente" um reflexo
    // aqui a partir só dos vizinhos — isso vazaria reflexo pra fora da silhueta do hit original.
    if (center.a <= 0.0) {
        FragColor = vec4(0.0);
        return;
    }

    float roughness = clamp(texture(normalRoughTexture, TexCoords).a, 0.0, 1.0);
    float radiusPx = roughness * maxBlurRadius;

    // Reflexo praticamente espelhado: não vale gastar as 24 amostras extras do kernel.
    if (radiusPx < 0.6) {
        FragColor = center;
        return;
    }

    vec2 texel = radiusPx / screenSize;

    vec3 colorSum = center.rgb * center.a;
    float alphaSum = center.a;
    float weightSum = 1.0;

    for (int x = -2; x <= 2; x++) {
        for (int y = -2; y <= 2; y++) {
            if (x == 0 && y == 0) continue;
            vec4 s = texture(ssrInput, TexCoords + vec2(float(x), float(y)) * texel);
            colorSum += s.rgb * s.a;
            alphaSum += s.a;
            weightSum += 1.0;
        }
    }

    float finalAlpha = alphaSum / weightSum;
    vec3 finalColor = (alphaSum > 0.0001) ? (colorSum / alphaSum) : vec3(0.0);

    FragColor = vec4(finalColor, finalAlpha);
}
)";

// Funde o resultado do ray march já borrado (ssrBlurColorBuffer) com a cena (mainColorBuffer),
// ponderado pela roughness/metallic da superfície (vindos do G-buffer-lite) e pela confiança
// do hit. Escreve em ssrCompositeColorBuffer (FBO dedicado) — nunca em mainColorBuffer
// diretamente, já que este último é uma das texturas de entrada lidas no mesmo draw call
// (ler e escrever a mesma textura simultaneamente é UB em OpenGL). O chamador (PostProcessing)
// copia o resultado de volta para mainColorBuffer com um blit logo em seguida.
const char* ssrCompositeFragment = R"(
#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D sceneColorTexture;
uniform sampler2D ssrTexture;
uniform sampler2D normalRoughTexture;
uniform sampler2D depthTexture;
uniform sampler2D metallicTexture;
uniform mat4 invProjection;
uniform float reflectionsStrength;

vec3 ViewPosFromDepth(vec2 uv, float depth) {
    vec4 clip = vec4(uv * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    vec4 view = invProjection * clip;
    return view.xyz / view.w;
}

void main() {
    vec3 sceneColor = texture(sceneColorTexture, TexCoords).rgb;
    vec4 ssr = texture(ssrTexture, TexCoords);
    vec4 nr = texture(normalRoughTexture, TexCoords);

    if (ssr.a <= 0.0 || nr.a < 0.0) {
        FragColor = vec4(sceneColor, 1.0);
        return;
    }

    float roughness = clamp(nr.a, 0.0, 1.0);
    // Reduz a intensidade conforme a rugosidade aumenta: um SSR "espelhado" sobre uma
    // superfície rugosa não é fisicamente coerente (precisaria de um blur por roughness,
    // que este passe simples não faz), então deixamos a contribuição morrer suavemente
    // e o specular_ibl (já calculado por material) assume o protagonismo nesse caso.
    // Janela mais larga (0.55→0.95) que antes: superfícies de roughness média (chão molhado,
    // asfalto, poças — tipicamente 0.3~0.6) mantêm reflexo visível em vez de já começarem
    // esmaecidas a partir de 0.35.
    float roughnessFade = 1.0 - smoothstep(0.55, 0.95, roughness);

    // Fresnel real (Schlick + roughness): sem isso, o reflexo aparece com a mesma força
    // olhando de frente ou de raspão para a superfície, o que faz a cor refletida (ex.: chão
    // marrom atrás do carro) parecer um "decalque" colado na lataria em vez de um reflexo que
    // só deveria se intensificar perto das bordas/ângulos rasantes da carroceria.
    //
    // F0 físico (igual ao F0_base do shader de material, mix(0.04, baseColor, metallic)):
    // dielétrico fica em ~0.04, condutor/metal usa a própria reflectância de banda larga, bem
    // mais alta mesmo de frente (NdotV alto) — metal não tem termo difuso, então essencialmente
    // toda a resposta visual do material vem do reflexo. Não temos o baseColor exato aqui (já
    // foi consumido pelo shading direto/IBL antes deste passe), só o canal metallic do
    // G-buffer-lite — por isso aproximamos a magnitude do F0 metálico por um escalar alto em
    // vez de tingir com a cor do material.
    //
    // IMPORTANTE: o termo de Fresnel abaixo usa max(1.0 - roughness, F0) em vez de só
    // (1.0 - F0) como "teto" do realce de borda — é a MESMA fórmula que fresnelSchlickRoughness
    // já usa pro specular_ibl (logo acima no arquivo, na ambient contribution). Isso faz
    // roughness e metallic atuarem no MESMO termo, não em dois multiplicadores desconexos:
    // conforme a superfície fica mais rugosa, o pico de brilho na borda (grazing angle) encolhe
    // e some — uma superfície 100% rugosa não tem mais aquele "aro" de Fresnel, fica uniforme
    // em F0 independente do ângulo, exatamente como esperado fisicamente.
    float depth = texture(depthTexture, TexCoords).r;
    vec3 viewPos = ViewPosFromDepth(TexCoords, depth);
    vec3 V = normalize(-viewPos);
    vec3 N = normalize(nr.rgb);
    float NdotV = clamp(dot(N, V), 0.0, 1.0);

    float metallic = clamp(texture(metallicTexture, TexCoords).r, 0.0, 1.0);
    float F0_scalar = mix(0.04, 0.9, metallic);
    float fresnel = F0_scalar + (max(1.0 - roughness, F0_scalar) - F0_scalar) * pow(1.0 - NdotV, 5.0);
    // O termo de Fresnel acima já é fisicamente completo (vai de F0_scalar de frente até
    // ~(1-roughness) em ângulo rasante). O piso do realce de borda precisa escalar com o
    // próprio F0_scalar do material, e não ser um valor fixo: um piso fixo (ex.: 0.5)
    // sobrescreve o F0 baixo de dielétricos (tinta, plástico, ~0.04) e força um reflexo
    // perceptível mesmo olhando de frente para a lataria — exatamente o "decalque" de chão
    // refletido indesejavelmente na carroceria. Usando F0_scalar como piso, a reflexão de
    // frente cai para perto do F0 real do material (quase nula em dielétricos, alta em
    // metais) e só cresce de fato perto da borda/ângulo rasante, como o Fresnel físico prevê.
    float fresnelFactor = mix(F0_scalar, 1.0, fresnel);

    float blend = clamp(ssr.a * roughnessFade * fresnelFactor * reflectionsStrength, 0.0, 1.0);
    vec3 result = mix(sceneColor, ssr.rgb, blend);

    FragColor = vec4(result, 1.0);
}
)";



// ============================================================
// IBL PRE-COMPUTATION SHADERS
// ============================================================

// Shared vertex: renders a unit cube for cubemap capture passes
const char* iblCubeVertex = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
out vec3 localPos;
uniform mat4 projection;
uniform mat4 view;
void main() {
    localPos = aPos;
    gl_Position = projection * view * vec4(aPos, 1.0);
}
)";

// --- Pass 1: Equirectangular HDR → Cubemap ---
const char* iblEquirectToCubeFragment = R"(
#version 330 core
out vec4 FragColor;
in vec3 localPos;
uniform sampler2D equirectangularMap;
const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v) {
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}
void main() {
    vec2 uv = SampleSphericalMap(normalize(localPos));
    vec3 color = texture(equirectangularMap, uv).rgb;
    FragColor = vec4(color, 1.0);
}
)";

// --- Pass 2: Irradiance Convolution (diffuse IBL) ---
const char* iblIrradianceFragment = R"(
#version 330 core
out vec4 FragColor;
in vec3 localPos;
uniform samplerCube environmentMap;
const float PI = 3.14159265359;
void main() {
    vec3 N = normalize(localPos);
    vec3 up    = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, N));
    up         = normalize(cross(N, right));

    vec3 irradiance = vec3(0.0);
    float sampleDelta = 0.025;
    float nrSamples = 0.0;
    // IBL_BAKE_MAX: caps the raw HDR environment sample so that a single
    // extremely bright sun disk (50 000+ nits) cannot dominate the entire
    // irradiance integral, which would flood every surface with blown-out
    // energy and invert through ACES into the characteristic black artifact.
    const float IBL_BAKE_MAX = 20.0;

    for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta) {
        for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta) {
            // spherical to cartesian (in tangent space)
            vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            // tangent to world
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;
            irradiance += min(texture(environmentMap, sampleVec).rgb, vec3(IBL_BAKE_MAX)) * cos(theta) * sin(theta);
            nrSamples++;
        }
    }
    irradiance = PI * irradiance * (1.0 / float(nrSamples));
    FragColor = vec4(irradiance, 1.0);
}
)";

// --- Pass 3: Prefiltered Environment Map (specular IBL, roughness mips) ---
const char* iblPrefilterFragment = R"(
#version 330 core
out vec4 FragColor;
in vec3 localPos;
uniform samplerCube environmentMap;
uniform float roughness;
const float PI = 3.14159265359;

float DistributionGGX_IBL(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return a2 / denom;
}

float RadicalInverse_VdC(uint bits) {
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10;
}

vec2 Hammersley(uint i, uint N) {
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}

vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness) {
    float a = roughness * roughness;
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
    vec3 H = vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
    vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent   = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);
    return normalize(tangent * H.x + bitangent * H.y + N * H.z);
}

void main() {
    vec3 N = normalize(localPos);
    vec3 R = N;
    vec3 V = R;
    const uint SAMPLE_COUNT = 1024u;
    vec3 prefilteredColor = vec3(0.0);
    float totalWeight = 0.0;
    for (uint i = 0u; i < SAMPLE_COUNT; ++i) {
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        vec3 H  = ImportanceSampleGGX(Xi, N, roughness);
        vec3 L  = normalize(2.0 * dot(V, H) * H - V);
        float NdotL = max(dot(N, L), 0.0);
        if (NdotL > 0.0) {
            float D   = DistributionGGX_IBL(N, H, roughness);
            float NdotH = max(dot(N, H), 0.0);
            float HdotV = max(dot(H, V), 0.0);
            float pdf   = D * NdotH / (4.0 * HdotV) + 0.0001;
            float resolution = 512.0;
            float saTexel  = 4.0 * PI / (6.0 * resolution * resolution);
            float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);
            float mipLevel = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);
            prefilteredColor += min(textureLod(environmentMap, L, mipLevel).rgb, vec3(20.0)) * NdotL;
            totalWeight      += NdotL;
        }
    }
    prefilteredColor = prefilteredColor / totalWeight;
    FragColor = vec4(prefilteredColor, 1.0);
}
)";

// --- Pass 4: BRDF Integration LUT ---
const char* iblBrdfLUTVertex = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;
out vec2 TexCoords;
void main() {
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)";

const char* iblBrdfLUTFragment = R"(
#version 330 core
out vec2 FragColor;
in vec2 TexCoords;
const float PI = 3.14159265359;

float RadicalInverse_VdC(uint bits) {
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10;
}
vec2 Hammersley(uint i, uint N) {
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness) {
    float a = roughness * roughness;
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
    vec3 H = vec3(cos(phi)*sinTheta, sin(phi)*sinTheta, cosTheta);
    vec3 up = abs(N.z) < 0.999 ? vec3(0,0,1) : vec3(1,0,0);
    vec3 tangent   = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);
    return normalize(tangent * H.x + bitangent * H.y + N * H.z);
}
float GeometrySchlickGGX_IBL(float NdotV, float roughness) {
    float a = roughness;
    float k = (a * a) / 2.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}
float GeometrySmith_IBL(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    return GeometrySchlickGGX_IBL(NdotV, roughness) * GeometrySchlickGGX_IBL(NdotL, roughness);
}
vec2 IntegrateBRDF(float NdotV, float roughness) {
    vec3 V = vec3(sqrt(1.0 - NdotV*NdotV), 0.0, NdotV);
    float A = 0.0, B = 0.0;
    vec3 N = vec3(0,0,1);
    const uint SAMPLE_COUNT = 1024u;
    for (uint i = 0u; i < SAMPLE_COUNT; ++i) {
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        vec3 H  = ImportanceSampleGGX(Xi, N, roughness);
        vec3 L  = normalize(2.0 * dot(V, H) * H - V);
        float NdotL = max(L.z, 0.0);
        float NdotH = max(H.z, 0.0);
        float VdotH = max(dot(V, H), 0.0);
        if (NdotL > 0.0) {
            float G     = GeometrySmith_IBL(N, V, L, roughness);
            float G_Vis = (G * VdotH) / (NdotH * NdotV);
            float Fc    = pow(1.0 - VdotH, 5.0);
            A += (1.0 - Fc) * G_Vis;
            B += Fc * G_Vis;
        }
    }
    return vec2(A, B) / float(SAMPLE_COUNT);
}
void main() {
    FragColor = IntegrateBRDF(TexCoords.x, TexCoords.y);
}
)";

// ============================================================
// SKYBOX (unchanged)
// ============================================================
const char* hdrBgVertex = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = aPos;

    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww; // truque skybox
}
)";

const char* hdrBgFragment = R"(
#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform sampler2D hdrTex;
uniform float exposure;

const vec2 invAtan = vec2(0.1591, 0.3183);

vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{
    vec2 uv = SampleSphericalMap(normalize(TexCoords));
    vec3 color = texture(hdrTex, uv).rgb;

    color *= exposure;

    // --- TONEMAP (exponential, estável e barato) ---
    vec3 mapped = vec3(1.0) - exp(-color);

    // --- GAMMA CORRECTION ---
    mapped = pow(mapped, vec3(1.0 / 2.2));

    FragColor = vec4(mapped, 1.0);
}
)";