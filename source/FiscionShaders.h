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

layout(location = 0) in vec3 aPos;
layout(location = 4) in uvec4 aJoint;
layout(location = 5) in vec4  aWeight;

layout(std140, binding = 0) uniform Skin { mat4 bones[100]; };

uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main() {
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

uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main() {
	gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}
)";

const char* depthCubeSkinnedVertex = R"(
#version 420 core

layout(location = 0) in vec3  aPos;
layout(location = 4) in uvec4 aJoint;
layout(location = 5) in vec4  aWeight;

layout(std140, binding = 0) uniform Skin { mat4 bones[100]; };

uniform mat4 model;
uniform mat4 shadowMatrices[15];

void main() {
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

uniform mat4 model;
uniform mat4 shadowMatrices[15];

void main() {
	vec4 worldPos = model * vec4(aPos, 1.0);
	gl_Position = shadowMatrices[gl_InstanceID] * worldPos;
}
)";

const char* depth_fragment = R"(
#version 420 core
void main() {}
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

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec3 Tangent;
    vec3 Bitangent;
    vec2 TexCoords;
    vec4 FragPosLightSpace[15];
} fs_in;

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

const float diskRadius = 0.1;

vec4 baseSample = vec4(1.0);

const vec2 poisson8[8] = vec2[](
    vec2(-0.326212, -0.40581),
    vec2(-0.840144,  0.07358),
    vec2(-0.695914, -0.45755),
    vec2(-0.203345,  0.620716),
    vec2( 0.96234,  -0.194983),
    vec2( 0.473434, -0.480026),
    vec2( 0.519456,  0.767022),
    vec2( 0.185461, -0.893124)
);

float random(vec3 seed, int i) {
    vec4 seed4 = vec4(seed, i);
    float dot_product = dot(seed4, vec4(12.9898, 78.233, 45.164, 94.673));
    return fract(sin(dot_product) * 43758.5453);
}

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

    vec4 fragPosLS = cascadeLightSpaceMatrices[layer] * vec4(fragPosWorld, 1.0);
    vec3 projCoords = fragPosLS.xyz / fragPosLS.w;
    projCoords = projCoords * 0.5 + 0.5;

    float currentDepth = projCoords.z;
    if (currentDepth > 1.0) return 0.0;

    float bias = max(0.0007 * (1.0 - dot(N, L)), 0.00007);
    if (layer == cascadeCount - 1) bias *= 0.5; 
    else bias *= 1.0 / (float(layer) + 1.0); 

    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(shadowMapDir, 0));
    
    float noise = interleavedGradientNoise(gl_FragCoord.xy);
    float angle = noise * 2.0 * PI;
    float s = sin(angle);
    float c = cos(angle);
    mat2 rotation = mat2(c, -s, s, c);

    float spread = 1.5; 

    for(int i = 0; i < 16; ++i) {
        vec2 offset = (rotation * poisson16[i]) * texelSize * spread;
        float pcfDepth = texture(shadowMapDir, vec3(projCoords.xy + offset, layer)).r;
        shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;
    }

    return shadow / 16.0;
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
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / max(denom, 0.0000001);
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0; 
    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    return nom / max(denom, 0.0000001);
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
  // NÃO aplicar pow(2.2) aqui: as texturas de cor são carregadas com GL_SRGB8_ALPHA8 /
  // GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM, então o OpenGL já converte sRGB→linear
  // automaticamente na leitura. Aplicar de novo causaria double-degamma (ficaria laranja).

  float metallic;
  float roughness;
  vec3  F0_base;

  if (useMetalRoughness == 1) {
    vec4 metallicProps = texture(metallicTex, fs_in.TexCoords);

    // glTF spec: roughness no canal G, metallic no canal B.
    // Os fatores do material MULTIPLICAM o valor da textura (Blender exporta assim).
    roughness = metallicProps.g * roughnessFactor;
    metallic  = metallicProps.b * metallicFactor;

    F0_base = mix(vec3(0.04), baseColor, metallic);
  } else if (hasGlossinessMap == 1) {
    // KHR_materials_pbrSpecularGlossiness:
    // - specularGlossinessTexture: RGB = especular F0 (sRGB), A = glossiness (linear)
    // - glossiness está no canal A da textura combinada (specularGlossiness)
    float glossiness = texture(glossinessTex, fs_in.TexCoords).a;
    roughness = 1.0 - glossiness;
    // metallic não existe neste workflow — usar 0 para não suprimir kD
    metallic = 0.0;

    if (hasSpecularF0Map == 1) {
      // Os canais RGB da specularGlossinessTexture já vêm linearizados pelo GL_SRGB
      F0_base = texture(specularF0Tex, fs_in.TexCoords).rgb;
    } else {
      F0_base = vec3(0.04);
    }
  } else {
    // Sem textura: apenas fatores escalares do material
    metallic  = metallicFactor;
    roughness = roughnessFactor;
    F0_base   = mix(vec3(0.04), baseColor, metallic);
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
  } else {
    N = normalize(fs_in.Normal);
  }

  vec3 V = normalize(viewPos - fs_in.FragPos);

  vec3 ambientSum = vec3(0.0);
  vec3 directLightSum = vec3(0.0);

  vec3 ambientContribution;
  if (hasIBL == 1) {
    // iblScale = 1.0 para ambos os workflows: não penalizar glossiness arbitrariamente
    float iblScale = 1.0;

    vec3 kS_amb = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0_base, roughness);
    vec3 kD_amb = (vec3(1.0) - kS_amb) * (1.0 - metallic);
    // Clamp raw IBL samples to prevent astronomically-bright HDR sun pixels
    // (which survive convolution as high-energy spikes) from overflowing the
    // PBR accumulator and inverting through ACES into a black artifact.
    const float IBL_SAMPLE_MAX = 20.0;
    vec3 irradiance = min(texture(irradianceMap, N).rgb, vec3(IBL_SAMPLE_MAX));
    vec3 diffuse_ibl = irradiance * baseColor;

    const float MAX_REFLECTION_LOD = 4.0;
    vec3 R = reflect(-V, N);
    vec3 prefilteredColor = min(textureLod(prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb, vec3(IBL_SAMPLE_MAX));
    vec2 envBRDF = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular_ibl = prefilteredColor * (F0_base * envBRDF.x + envBRDF.y) * reflectionsStrength;

    ambientContribution = (kD_amb * diffuse_ibl + specular_ibl) * environmentStrength;
  } else {
    float hemi = 0.5 * (dot(N, vec3(0, 1, 0)) + 1.0);
    vec3 ambientHemi = mix(environmentGroundColor, environmentSkyColor, hemi);
    ambientContribution = baseColor * ambientHemi * environmentStrength;
  }

  // Cap the IBL contribution luminance to avoid NaN / overflow / ACES inversion
  // when the HDR environment contains an extremely bright sun disk (tens of thousands
  // of nits). The irradiance convolution spreads that energy everywhere, so every
  // surface facing the sun can receive values >>1 that cause ACES to produce values
  // slightly above 1.0 per-channel, resulting in the characteristic black artifact.
  // We cap BEFORE multiplying by environmentStrength so the tonemap input stays sane.
  const float IBL_MAX = 10.0;
  ambientContribution = min(ambientContribution, vec3(IBL_MAX));

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
        ambientSum += localAmbientFactor * baseColor * lightCol * intensity; // attenuation == 1 para directional
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
    float NdotH = max(dot(N, H), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float VdotH = max(dot(V, H), 0.0);
    
    vec3 F = fresnelSchlick(VdotH, F0_base);
    
    float D = DistributionGGX(N, H, roughness);
    
    float G = GeometrySmith(N, V, L, roughness);

    vec3 specularBRDF = (D * G * F) / max(4.0 * NdotL * max(dot(N, V), 0.0), 0.000001);
    specularBRDF *= reflectionsStrength; // <-- aqui

    vec3 kS = F;
    vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);
    vec3 diffuse = kD * baseColor / PI;
    
    vec3 directContrib = (diffuse + specularBRDF) * lightCol * NdotL * intensity;

    float shadowTerm = 1.0 - shadow;

    directLightSum += attenuation * directContrib * shadowTerm;
    
    ambientSum += attenuation * localAmbientFactor * baseColor * lightCol * intensity;
  }

  float transAmt = texture(transmissionTex, fs_in.TexCoords).r * transmissionFactor;
  vec3 transLight = baseColor * transAmt * 0.5;

  vec3 result = ambientContribution + ambientSum + directLightSum;

  result = mix(result, result + transLight, transAmt);

  // The FBO is GL_RGB LDR. PBR produces HDR linear values — must tonemap + gamma
  // here so the stored pixel is already correct sRGB, consistent with the skybox.
  // ACES Filmic (Narkowicz 2015): compresses highlights without oversaturating.
  #define ACES(c) clamp(((c)*(2.51*(c)+0.03))/((c)*(2.43*(c)+0.59)+0.14),0.0,1.0)
  #define TO_SRGB(c) pow(max((c),vec3(0.0)),vec3(1.0/2.2))

  if (isAffectedByLight == 0) {
    vec3 fallback = ambientContribution + ambientSum;
    fallback = max(fallback, baseColor * 0.7);
    fallback = mix(fallback, fallback + transLight, transAmt);
    FragColor = vec4(TO_SRGB(ACES(fallback * hdrExposure)), (alphaMode == 2 ? baseSample.a : 1.0));
  } else {
    FragColor = vec4(TO_SRGB(ACES(result * hdrExposure)), (alphaMode == 2 ? baseSample.a : 1.0));
  }

  #undef ACES
  #undef TO_SRGB
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
    FragColor = vec4(color.rgb, color.a * alpha);
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
    FragColor = vec4(color.rgb, color.a * alpha);
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
    vec4 skinned = skinPosition(vec4(aPos, 1.0));
    vec4 worldPos = model * skinned;
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

// OPTIM: normalMatrix is precomputed on the CPU and uploaded as a uniform
// instead of calling transpose(inverse(model)) per-vertex on the GPU.
// mat3(transpose(inverse(model))) involves a full 4x4 matrix inverse inside
// every vertex invocation — that is O(N_verts) matrix inversions per frame.
// The CPU computes it once per draw call and sends it via the "normalMatrix" uniform.
// NOTE: Add "uniform mat3 normalMatrix;" to vertexStatic and vertexSkinned,
//       and set it from CPU before each drawSubMesh call.  If non-uniform scale
//       is never used, mat3(model) is also acceptable (avoids the inverse entirely).
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

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrices[15]; // um por luz
// OPTIM: normalMatrix precomputed on CPU — avoids transpose(inverse(model)) per vertex
uniform mat3 normalMatrix;

void main() {
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
    //float ang = atan(uv_corr.y - sunPos_corr.y, uv_corr.x - sunPos_corr.x);
    //float star = pow(sin(ang * 6.0 + 1.5), 10.0) * 0.5 + pow(sin(ang * 10.0), 10.0) * 0.3;
    //flareFinal += star * sunColor * exp(-distToSun * 10.0) * 2.0;

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

    // --- EXPOSURE ---
    color *= exposure;

    // --- TONEMAP (exponential, estável e barato) ---
    vec3 mapped = vec3(1.0) - exp(-color);

    // --- GAMMA CORRECTION ---
    mapped = pow(mapped, vec3(1.0 / 2.2));

    FragColor = vec4(mapped, 1.0);
}
)";