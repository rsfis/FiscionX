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
// /*
const char* fragment = R"(
#version 330 core

// tipos de luz
const int LIGHT_DIRECTIONAL = 0;
const int LIGHT_POINT = 1;
const int LIGHT_SPOT = 2;

// PCF omnidirecional para point lights
const int PCF_SAMPLES = 20;
const vec3 gridSamplingOffset[PCF_SAMPLES] = vec3[](
  vec3(0.5381, 0.1856, -0.4319), vec3(0.1379, 0.2486, 0.4430),
  vec3(0.3371, 0.5679, -0.0057), vec3(-0.6999, -0.0451, -0.0019),
  vec3(0.0689, -0.1598, -0.8547), vec3(0.0560, 0.0069, -0.1843),
  vec3(-0.0146, 0.1402, 0.0762), vec3(0.0100, -0.1924, -0.0344),
  vec3(-0.3577, -0.5301, -0.4358), vec3(-0.3169, 0.1063, 0.0158),
  vec3(0.0103, -0.5869, 0.0046), vec3(-0.0897, -0.4940, 0.3287),
  vec3(0.7119, -0.0154, -0.0918), vec3(-0.0533, 0.0596, -0.5411),
  vec3(0.0352, -0.0631, 0.5460), vec3(-0.4776, 0.2847, -0.0271),
  vec3(0.2083, -0.6554, 0.4233), vec3(-0.2899, 0.7535, -0.2406),
  vec3(0.4190, 0.1449, 0.3743), vec3(-0.5871, -0.4252, 0.1035)
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

uniform sampler2D baseColorTex;
uniform sampler2D normalMapTex;
uniform sampler2D glossinessTex;
uniform sampler2D specularF0Tex;
uniform int hasGlossinessMap;
uniform int hasSpecularF0Map;
uniform sampler2D shadowMaps[15];
uniform samplerCube shadowCubeMaps[15];
uniform sampler2D transmissionTex;

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

uniform float alpha;

const float diskRadius = 0.1;

float ShadowCalculation2D(vec4 fragPosLS, sampler2D shadowMap, vec3 N, vec3 L) {
    if (acceptsShadows == 0) return 0.0;

    vec3 projCoords = fragPosLS.xyz / fragPosLS.w;
    projCoords = projCoords * 0.5 + 0.5;

    // fora do shadowmap → sem sombra
    if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.y < 0.0 ||
        projCoords.x > 1.0 || projCoords.y > 1.0) 
        return 0.0;

    float currentDepth = projCoords.z;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    // bias adaptativo
    float bias = max(0.003 * (1.0 - dot(N, L)), 0.001);

    // raio ajustado pela distância
    float distFactor = clamp(currentDepth * 6.0, 1.0, 4.0);
    float radius = 1.5 * distFactor;

    const vec2 poissonDisk[32] = vec2[](
        vec2(-0.613392, 0.617481), vec2(0.170019, -0.040254),
        vec2(-0.299417, -0.791925), vec2(0.645680, 0.493210),
        vec2(-0.651784, -0.717887), vec2(0.421003, -0.027070),
        vec2(-0.817194, 0.271096), vec2(0.676962, -0.611195),
        vec2(-0.205527, 0.780980), vec2(0.881396, -0.458965),
        vec2(-0.613392, 0.995210), vec2(0.156989, 0.819345),
        vec2(-0.102147, -0.171599), vec2(0.849747, 0.300822),
        vec2(-0.207641, -0.617362), vec2(0.320019, -0.920254),
        vec2(0.562210, -0.326998), vec2(-0.783316, -0.253210),
        vec2(0.230144, 0.305274), vec2(-0.185660, 0.147680),
        vec2(0.698422, -0.027080), vec2(-0.970138, 0.134580),
        vec2(0.198182, -0.577850), vec2(-0.451879, 0.353680),
        vec2(0.401144, -0.705274), vec2(-0.298182, 0.977850),
        vec2(0.870138, 0.284580), vec2(-0.598422, -0.127080),
        vec2(0.285660, 0.247680), vec2(-0.701144, 0.505274),
        vec2(0.498182, -0.377850), vec2(-0.870138, -0.384580)
    );

    float shadow = 0.0;
    for (int i = 0; i < 32; ++i) {
        vec2 offset = poissonDisk[i] * texelSize * radius;
        float closestDepth = texture(shadowMap, projCoords.xy + offset).r;
        if (currentDepth - bias > closestDepth)
            shadow += 1.0;
    }

    return shadow / 32.0; // fração de sombra
}

float ShadowCalculationPoint(int idx, vec3 fragPos) {
    if (acceptsShadows == 0) return 0.0;

    vec3 fragToLight = fragPos - lightPos[idx];
    float currentDepth = length(fragToLight);

    float bias = 0.05 + currentDepth * 0.005;

    float shadow = 0.0;
    for (int i = 0; i < PCF_SAMPLES; ++i) {
        float closestDepth = texture(
            shadowCubeMaps[idx],
            fragToLight + gridSamplingOffset[i] * diskRadius
        ).r;

        closestDepth *= lightMaxDistance[idx];
        if (currentDepth - bias > closestDepth)
            shadow += 1.0;
    }

    return shadow / float(PCF_SAMPLES); // fração de sombra
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
  return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main() {
  vec4 baseSample = texture(baseColorTex, fs_in.TexCoords);
  if (alphaMode == 1 && baseSample.a < alphaCutoff) discard;
  vec3 baseColor = baseSample.rgb;

  float glossiness = hasGlossinessMap == 1 ? clamp(texture(glossinessTex, fs_in.TexCoords).r, 0.05, 1.0) : 1.0;
  vec3 F0 = vec3(0.04);
  if (hasSpecularF0Map == 1) {
    vec3 texF0 = texture(specularF0Tex, fs_in.TexCoords).rgb;
    if (length(texF0) > 0.01) F0 = clamp(texF0, vec3(0.0), vec3(0.5));
  }

  vec3 tangentNormal = hasNormalMap == 1 ? texture(normalMapTex, fs_in.TexCoords).rgb * 2.0 - 1.0 : vec3(0, 0, 1);
  mat3 TBN = mat3(normalize(fs_in.Tangent), normalize(fs_in.Bitangent), normalize(fs_in.Normal));
  vec3 N = normalize(TBN * tangentNormal);

  vec3 ambientSum = vec3(0.0);
  vec3 diffuseSum = vec3(0.0);

  for (int i = 0; i < numLights; ++i) {
    vec3 L;
    float attenuation = 1.0, intensity = 1.0, shadow = 0.0;

    if (lightType[i] == LIGHT_DIRECTIONAL) {
      L = normalize(-lightDir[i]);
      shadow = ShadowCalculation2D(fs_in.FragPosLightSpace[i], shadowMaps[i], N, L);
    } else if (lightType[i] == LIGHT_POINT) {
      vec3 toLight = lightPos[i] - fs_in.FragPos;
      float dist = length(toLight);
      if (dist > lightMaxDistance[i]) continue;
      L = normalize(toLight);
      attenuation = pow(clamp(1.0 - dist / lightMaxDistance[i], 0.0, 1.0), 2.0);
      shadow = ShadowCalculationPoint(i, fs_in.FragPos);
    } else {
      vec3 toLight = lightPos[i] - fs_in.FragPos;
      float dist = length(toLight);
      if (dist > lightMaxDistance[i]) continue;
      L = normalize(toLight);
      attenuation = 1.0 / (lightConstant[i] + lightLinear[i] * dist + lightQuadratic[i] * dist * dist);
      float theta = dot(-L, normalize(lightDir[i]));
      float eps = max(lightCutOff[i] - lightOuterCutOff[i], 0.001);
      intensity = clamp((theta - lightOuterCutOff[i]) / eps, 0.0, 1.0);
      if (theta < lightOuterCutOff[i]) continue;
      shadow = ShadowCalculation2D(fs_in.FragPosLightSpace[i], shadowMaps[i], N, L);
    }

    float diff = max(dot(N, L), 0.0);
    vec3 V = normalize(viewPos - fs_in.FragPos);
    vec3 H = normalize(L + V);
    float NdotH = max(dot(N, H), 0.0);

    vec3 kS = fresnelSchlick(NdotH, F0);
    vec3 kD = max(vec3(1.0) - kS, vec3(0.05));

    vec3 diffuse = kD * baseColor * diff;
    vec3 specular = kS * pow(NdotH, glossiness * 128.0);
    vec3 lightCol = lightColor[i] * lightIntensity[i];
    vec3 contrib = (diffuse + specular) * (1.0 - shadow);

    diffuseSum += attenuation * intensity * contrib * lightCol;
    ambientSum += attenuation * intensity * 0.05 * baseColor * lightCol;
  }

  float transAmt = texture(transmissionTex, fs_in.TexCoords).r * transmissionFactor;
  vec3 transLight = baseColor * transAmt * 0.5;
  vec3 result = ambientSum + diffuseSum;

  // environment hemispheric light
  float hemi = 0.5 * (dot(N, vec3(0, 1, 0)) + 1.0);
  vec3 ambientHemi = mix(environmentGroundColor, environmentSkyColor, hemi);
  result += ambientHemi * baseColor * environmentStrength * 1.3;

  // compensar areas muito escuras
  result = max(result, baseColor * 0.7);
  result = mix(result, result + transLight, transAmt);

  if (isAffectedByLight == 0) {
    result = ambientSum;
    result += baseColor * ambientHemi * environmentStrength * 1.3;
    result = max(result, baseColor * 0.7);
    result = mix(result, result + transLight, transAmt);
    FragColor = vec4(result, (alphaMode == 2 ? baseSample.a : 1.0) * alpha);
  } else {
    FragColor = vec4(result, (alphaMode == 2 ? baseSample.a : 1.0) * alpha);
  }
}
)";
// */

// For FiscionX Universal Render Pipeline
/*
const char* fragment = R"(
#version 330 core

// tipos de luz
const int LIGHT_DIRECTIONAL = 0;
const int LIGHT_POINT = 1;
const int LIGHT_SPOT = 2;

// PCF omnidirecional para point lights
const int PCF_SAMPLES = 20;
const vec3 gridSamplingOffset[PCF_SAMPLES] = vec3[](
  vec3(0.5381, 0.1856, -0.4319), vec3(0.1379, 0.2486, 0.4430),
  vec3(0.3371, 0.5679, -0.0057), vec3(-0.6999, -0.0451, -0.0019),
  vec3(0.0689, -0.1598, -0.8547), vec3(0.0560, 0.0069, -0.1843),
  vec3(-0.0146, 0.1402, 0.0762), vec3(0.0100, -0.1924, -0.0344),
  vec3(-0.3577, -0.5301, -0.4358), vec3(-0.3169, 0.1063, 0.0158),
  vec3(0.0103, -0.5869, 0.0046), vec3(-0.0897, -0.4940, 0.3287),
  vec3(0.7119, -0.0154, -0.0918), vec3(-0.0533, 0.0596, -0.5411),
  vec3(0.0352, -0.0631, 0.5460), vec3(-0.4776, 0.2847, -0.0271),
  vec3(0.2083, -0.6554, 0.4233), vec3(-0.2899, 0.7535, -0.2406),
  vec3(0.4190, 0.1449, 0.3743), vec3(-0.5871, -0.4252, 0.1035)
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

uniform sampler2D baseColorTex;
uniform sampler2D normalMapTex;
uniform sampler2D glossinessTex;
uniform sampler2D specularF0Tex;
uniform int hasGlossinessMap;
uniform int hasSpecularF0Map;
uniform sampler2D shadowMaps[15];
uniform samplerCube shadowCubeMaps[15];
uniform sampler2D transmissionTex;

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

uniform float alpha;

const float diskRadius = 0.1;

float ShadowCalculation2D(vec4 fragPosLS, sampler2D shadowMap, vec3 N, vec3 L) {
    if (acceptsShadows == 0) return 0.0;

    vec3 projCoords = fragPosLS.xyz / fragPosLS.w;
    projCoords = projCoords * 0.5 + 0.5;

    // fora do shadowmap → sem sombra
    if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.y < 0.0 ||
        projCoords.x > 1.0 || projCoords.y > 1.0)
        return 0.0;

    float currentDepth = projCoords.z;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    // bias adaptativo
    float bias = max(0.003 * (1.0 - dot(N, L)), 0.001);

    // raio ajustado pela distância
    float distFactor = clamp(currentDepth * 6.0, 1.0, 4.0);
    float radius = 1.5 * distFactor;

    const vec2 poissonDisk[32] = vec2[](
        vec2(-0.613392, 0.617481), vec2(0.170019, -0.040254),
        vec2(-0.299417, -0.791925), vec2(0.645680, 0.493210),
        vec2(-0.651784, -0.717887), vec2(0.421003, -0.027070),
        vec2(-0.817194, 0.271096), vec2(0.676962, -0.611195),
        vec2(-0.205527, 0.780980), vec2(0.881396, -0.458965),
        vec2(-0.613392, 0.995210), vec2(0.156989, 0.819345),
        vec2(-0.102147, -0.171599), vec2(0.849747, 0.300822),
        vec2(-0.207641, -0.617362), vec2(0.320019, -0.920254),
        vec2(0.562210, -0.326998), vec2(-0.783316, -0.253210),
        vec2(0.230144, 0.305274), vec2(-0.185660, 0.147680),
        vec2(0.698422, -0.027080), vec2(-0.970138, 0.134580),
        vec2(0.198182, -0.577850), vec2(-0.451879, 0.353680),
        vec2(0.401144, -0.705274), vec2(-0.298182, 0.977850),
        vec2(0.870138, 0.284580), vec2(-0.598422, -0.127080),
        vec2(0.285660, 0.247680), vec2(-0.701144, 0.505274),
        vec2(0.498182, -0.377850), vec2(-0.870138, -0.384580)
    );

    float shadow = 0.0;
    for (int i = 0; i < 32; ++i) {
        vec2 offset = poissonDisk[i] * texelSize * radius;
        float closestDepth = texture(shadowMap, projCoords.xy + offset).r;
        if (currentDepth - bias > closestDepth)
            shadow += 1.0;
    }

    return shadow / 32.0; // fração de sombra
}

float ShadowCalculationPoint(int idx, vec3 fragPos) {
    if (acceptsShadows == 0) return 0.0;

    vec3 fragToLight = fragPos - lightPos[idx];
    float currentDepth = length(fragToLight);

    float bias = 0.05 + currentDepth * 0.005;

    float shadow = 0.0;
    for (int i = 0; i < PCF_SAMPLES; ++i) {
        float closestDepth = texture(
            shadowCubeMaps[idx],
            fragToLight + gridSamplingOffset[i] * diskRadius
        ).r;

        closestDepth *= lightMaxDistance[idx];
        if (currentDepth - bias > closestDepth)
            shadow += 1.0;
    }

    return shadow / float(PCF_SAMPLES); // fração de sombra
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
  return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main() {
  vec4 baseSample = texture(baseColorTex, fs_in.TexCoords);
  if (alphaMode == 1 && baseSample.a < alphaCutoff) discard;
  vec3 baseColor = baseSample.rgb;

  float glossiness = hasGlossinessMap == 1 ? clamp(texture(glossinessTex, fs_in.TexCoords).r, 0.05, 1.0) : 1.0;
  vec3 F0 = vec3(0.04);
  if (hasSpecularF0Map == 1) {
    vec3 texF0 = texture(specularF0Tex, fs_in.TexCoords).rgb;
    if (length(texF0) > 0.01) F0 = clamp(texF0, vec3(0.0), vec3(0.5));
  }

  vec3 tangentNormal = hasNormalMap == 1 ? texture(normalMapTex, fs_in.TexCoords).rgb * 2.0 - 1.0 : vec3(0, 0, 1);
  mat3 TBN = mat3(normalize(fs_in.Tangent), normalize(fs_in.Bitangent), normalize(fs_in.Normal));
  vec3 N = normalize(TBN * tangentNormal);

  vec3 ambientSum = vec3(0.0);
  vec3 diffuseSum = vec3(0.0);

  for (int i = 0; i < numLights; ++i) {
    vec3 L;
    float attenuation = 1.0, intensity = 1.0, shadow = 0.0;

    if (lightType[i] == LIGHT_DIRECTIONAL) {
      L = normalize(-lightDir[i]);
      shadow = ShadowCalculation2D(fs_in.FragPosLightSpace[i], shadowMaps[i], N, L);
    } else if (lightType[i] == LIGHT_POINT) {
      vec3 toLight = lightPos[i] - fs_in.FragPos;
      float dist = length(toLight);
      if (dist > lightMaxDistance[i]) continue;
      L = normalize(toLight);
      attenuation = pow(clamp(1.0 - dist / lightMaxDistance[i], 0.0, 1.0), 2.0);
      shadow = ShadowCalculationPoint(i, fs_in.FragPos);
    } else {
      vec3 toLight = lightPos[i] - fs_in.FragPos;
      float dist = length(toLight);
      if (dist > lightMaxDistance[i]) continue;
      L = normalize(toLight);
      attenuation = 1.0 / (lightConstant[i] + lightLinear[i] * dist + lightQuadratic[i] * dist * dist);
      float theta = dot(-L, normalize(lightDir[i]));
      float eps = max(lightCutOff[i] - lightOuterCutOff[i], 0.001);
      intensity = clamp((theta - lightOuterCutOff[i]) / eps, 0.0, 1.0);
      if (theta < lightOuterCutOff[i]) continue;
      shadow = ShadowCalculation2D(fs_in.FragPosLightSpace[i], shadowMaps[i], N, L);
    }

    float diff = max(dot(N, L), 0.0);
    vec3 V = normalize(viewPos - fs_in.FragPos);
    vec3 H = normalize(L + V);
    float NdotH = max(dot(N, H), 0.0);

    vec3 kS = fresnelSchlick(NdotH, F0);
    vec3 kD = max(vec3(1.0) - kS, vec3(0.05));

    vec3 diffuse = kD * baseColor * diff;
    vec3 specular = kS * pow(NdotH, glossiness * 128.0);
    vec3 lightCol = lightColor[i] * lightIntensity[i];
    vec3 contrib = (diffuse + specular) * (1.0 - shadow);

    diffuseSum += attenuation * intensity * contrib * lightCol;
    ambientSum += attenuation * intensity * 0.05 * baseColor * lightCol;
  }

  float transAmt = texture(transmissionTex, fs_in.TexCoords).r * transmissionFactor;
  vec3 transLight = baseColor * transAmt * 0.5;
  vec3 result = ambientSum + diffuseSum;

  // environment hemispheric light
  float hemi = 0.5 * (dot(N, vec3(0, 1, 0)) + 1.0);
  vec3 ambientHemi = mix(environmentGroundColor, environmentSkyColor, hemi);
  result += ambientHemi * baseColor * environmentStrength * 1.3;

  // compensar areas muito escuras
  result = max(result, baseColor * 0.7);
  result = mix(result, result + transLight, transAmt);

  if (isAffectedByLight == 0) {
    result = ambientSum;
    result += baseColor * ambientHemi * environmentStrength * 1.3;
    result = max(result, baseColor * 0.7);
    result = mix(result, result + transLight, transAmt);
    FragColor = vec4(result, (alphaMode == 2 ? baseSample.a : 1.0) * alpha);
  } else {
    FragColor = vec4(result, (alphaMode == 2 ? baseSample.a : 1.0) * alpha);
  }
}
)";
*/

// For FiscionX Cel Shading Render Pipeline
/*
const char* fragment = R"(
#version 330 core

const int LIGHT_DIRECTIONAL = 0;
const int LIGHT_POINT = 1;
const int LIGHT_SPOT = 2;

const int PCF_SAMPLES = 20;
const vec3 gridSamplingOffset[PCF_SAMPLES] = vec3[](
  vec3(0.5381, 0.1856, -0.4319), vec3(0.1379, 0.2486, 0.4430),
  vec3(0.3371, 0.5679, -0.0057), vec3(-0.6999, -0.0451, -0.0019),
  vec3(0.0689, -0.1598, -0.8547), vec3(0.0560, 0.0069, -0.1843),
  vec3(-0.0146, 0.1402, 0.0762), vec3(0.0100, -0.1924, -0.0344),
  vec3(-0.3577, -0.5301, -0.4358), vec3(-0.3169, 0.1063, 0.0158),
  vec3(0.0103, -0.5869, 0.0046), vec3(-0.0897, -0.4940, 0.3287),
  vec3(0.7119, -0.0154, -0.0918), vec3(-0.0533, 0.0596, -0.5411),
  vec3(0.0352, -0.0631, 0.5460), vec3(-0.4776, 0.2847, -0.0271),
  vec3(0.2083, -0.6554, 0.4233), vec3(-0.2899, 0.7535, -0.2406),
  vec3(0.4190, 0.1449, 0.3743), vec3(-0.5871, -0.4252, 0.1035)
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

uniform sampler2D baseColorTex;
uniform sampler2D normalMapTex;
uniform sampler2D glossinessTex;
uniform sampler2D specularF0Tex;
uniform int hasGlossinessMap;
uniform int hasSpecularF0Map;
uniform sampler2D shadowMaps[15];
uniform samplerCube shadowCubeMaps[15];
uniform sampler2D transmissionTex;

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

uniform float alpha;
uniform float frameRand;
const float diskRadius = 0.1;

float ShadowCalculation2D(vec4 fragPosLS, sampler2D shadowMap, vec3 N, vec3 L) {
    if (acceptsShadows == 0) return 1.0;
    vec3 projCoords = fragPosLS.xyz / fragPosLS.w;
    projCoords = projCoords * 0.5 + 0.5;
    if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.y < 0.0 ||
        projCoords.x > 1.0 || projCoords.y > 1.0) return 0.0;

    float currentDepth = projCoords.z;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    const vec2 poissonDisk[32] = vec2[](
        vec2(-0.613392, 0.617481), vec2(0.170019, -0.040254),
        vec2(-0.299417, -0.791925), vec2(0.645680, 0.493210),
        vec2(-0.651784, -0.717887), vec2(0.421003, -0.027070),
        vec2(-0.817194, 0.271096), vec2(0.676962, -0.611195),
        vec2(-0.205527, 0.780980), vec2(0.881396, -0.458965),
        vec2(-0.613392, 0.995210), vec2(0.156989, 0.819345),
        vec2(-0.102147, -0.171599), vec2(0.849747, 0.300822),
        vec2(-0.207641, -0.617362), vec2(0.320019, -0.920254),
        vec2(0.562210, -0.326998), vec2(-0.783316, -0.253210),
        vec2(0.230144, 0.305274), vec2(-0.185660, 0.147680),
        vec2(0.698422, -0.027080), vec2(-0.970138, 0.134580),
        vec2(0.198182, -0.577850), vec2(-0.451879, 0.353680),
        vec2(0.401144, -0.705274), vec2(-0.298182, 0.977850),
        vec2(0.870138, 0.284580), vec2(-0.598422, -0.127080),
        vec2(0.285660, 0.247680), vec2(-0.701144, 0.505274),
        vec2(0.498182, -0.377850), vec2(-0.870138, -0.384580)
    );

    float angle = frameRand * 6.2831853;
    mat2 rot = mat2(cos(angle), -sin(angle), sin(angle), cos(angle));

    float distFactor = clamp(currentDepth * 6.0, 1.0, 4.0);
    float radius = 1.5 * distFactor;
    float bias = max(0.0015 * (1.0 - dot(N, L)), 0.0003);
    bias *= (radius * 0.35 + 1.0);

    float shadow = 0.0;
    for (int i = 0; i < 32; ++i) {
        vec2 offset = rot * poissonDisk[i] * texelSize * radius;
        float closestDepth = texture(shadowMap, projCoords.xy + offset).r;
        if (currentDepth - bias > closestDepth)
            shadow += 1.0;
    }

    return shadow / 32.0;
}

float ShadowCalculationPoint(int idx, vec3 fragPos) {
    if (acceptsShadows == 0) return 0.9;
    vec3 fragToLight = fragPos - lightPos[idx];
    float currentDepth = length(fragToLight);
    float bias = 0.1, shadow = 0.0;
    for (int i = 0; i < PCF_SAMPLES; ++i) {
        float closestDepth = texture(shadowCubeMaps[idx], fragToLight + gridSamplingOffset[i] * diskRadius).r;
        closestDepth *= lightMaxDistance[idx];
        if (currentDepth - bias > closestDepth) shadow += 1.0;
    }
    return shadow / float(PCF_SAMPLES);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
  return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main() {
  vec4 baseSample = texture(baseColorTex, fs_in.TexCoords);
  if (alphaMode == 1 && baseSample.a < alphaCutoff) discard;
  vec3 baseColor = baseSample.rgb;

  float glossiness = hasGlossinessMap == 1 ? clamp(texture(glossinessTex, fs_in.TexCoords).r, 0.05, 1.0) : 1.0;
  vec3 F0 = vec3(0.04);
  if (hasSpecularF0Map == 1) {
    vec3 texF0 = texture(specularF0Tex, fs_in.TexCoords).rgb;
    if (length(texF0) > 0.01) F0 = clamp(texF0, vec3(0.0), vec3(0.5));
  }

  vec3 tangentNormal = hasNormalMap == 1 ? texture(normalMapTex, fs_in.TexCoords).rgb * 2.0 - 1.0 : vec3(0, 0, 1);
  mat3 TBN = mat3(normalize(fs_in.Tangent), normalize(fs_in.Bitangent), normalize(fs_in.Normal));
  vec3 N = normalize(TBN * tangentNormal);

  vec3 ambientSum = vec3(0.0);
  vec3 diffuseSum = vec3(0.0);

  for (int i = 0; i < numLights; ++i) {
    vec3 L;
    float attenuation = 1.0, intensity = 1.0, shadow = 0.0;

    if (lightType[i] == LIGHT_DIRECTIONAL) {
      L = normalize(-lightDir[i]);
      shadow = ShadowCalculation2D(fs_in.FragPosLightSpace[i], shadowMaps[i], N, L);
    } else if (lightType[i] == LIGHT_POINT) {
      vec3 toLight = lightPos[i] - fs_in.FragPos;
      float dist = length(toLight);
      if (dist > lightMaxDistance[i]) continue;
      L = normalize(toLight);
      attenuation = pow(clamp(1.0 - dist / lightMaxDistance[i], 0.0, 1.0), 2.0);
      shadow = ShadowCalculationPoint(i, fs_in.FragPos);
    } else {
      vec3 toLight = lightPos[i] - fs_in.FragPos;
      float dist = length(toLight);
      if (dist > lightMaxDistance[i]) continue;
      L = normalize(toLight);
      attenuation = 1.0 / (lightConstant[i] + lightLinear[i] * dist + lightQuadratic[i] * dist * dist);
      float theta = dot(-L, normalize(lightDir[i]));
      float eps = max(lightCutOff[i] - lightOuterCutOff[i], 0.001);
      intensity = clamp((theta - lightOuterCutOff[i]) / eps, 0.0, 1.0);
      if (theta < lightOuterCutOff[i]) continue;
      shadow = ShadowCalculation2D(fs_in.FragPosLightSpace[i], shadowMaps[i], N, L);
    }

    // CEL SHADING: quantizar iluminação difusa
    float diff = dot(N, L);
    if (diff > 0.95) diff = 1.0;
    else if (diff > 0.5) diff = 0.7;
    else if (diff > 0.25) diff = 0.4;
    else diff = 0.1;

    vec3 V = normalize(viewPos - fs_in.FragPos);
    vec3 H = normalize(L + V);
    float NdotH = max(dot(N, H), 0.0);

    // Specular opcionalmente quantizado
    vec3 kS = fresnelSchlick(NdotH, F0);
    vec3 kD = max(vec3(1.0) - kS, vec3(0.05));
    vec3 specular = kS * pow(NdotH, glossiness * 128.0);
    // quantizar specular
    float specIntensity = length(specular);
    if (specIntensity > 0.5) specular *= 1.0;
    else specular *= 0.0;

    vec3 diffuse = kD * baseColor * diff;
    vec3 lightCol = lightColor[i] * lightIntensity[i];
    vec3 contrib = (diffuse + specular) * (1.0 - shadow);

    diffuseSum += attenuation * intensity * contrib * lightCol;
    ambientSum += attenuation * intensity * 0.05 * baseColor * lightCol;
  }

  vec3 result = ambientSum + diffuseSum;

  // ambient hemispheric
  float hemi = 0.5 * (dot(N, vec3(0, 1, 0)) + 1.0);
  vec3 ambientHemi = mix(environmentGroundColor, environmentSkyColor, hemi);
  result += ambientHemi * baseColor * environmentStrength * 1.3;

  // compensar areas muito escuras
  result = max(result, baseColor * 0.7);

  FragColor = vec4(result, (alphaMode == 2 ? baseSample.a : 1.0) * alpha);
}
)";
*/

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

void main()
{
    vec2 centered = (aPos - 0.5) * scale;

    mat2 rot = mat2(cos(rotation), -sin(rotation),
                    sin(rotation),  cos(rotation));
    vec2 rotated = rot * centered;

    vec2 translated = rotated + position;

    gl_Position = projection * vec4(translated, 0.0, 1.0);

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

    mat3 normalMatrix = mat3(transpose(inverse(model)));
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

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrices[15]; // um por luz

void main() {
    vec4 worldPos = model * vec4(aPos, 1.0);
    vs_out.FragPos = worldPos.xyz;

    mat3 normalMatrix = mat3(transpose(inverse(model)));
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