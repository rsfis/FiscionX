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

uniform float alpha;

const float diskRadius = 0.1;

float ShadowCalculation2D(vec4 fragPosLS, sampler2D shadowMap, vec3 N, vec3 L) {
  vec3 projCoords = fragPosLS.xyz / fragPosLS.w;
  projCoords = projCoords * 0.5 + 0.5;
  if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.y < 0.0 ||
      projCoords.x > 1.0 || projCoords.y > 1.0) return 0.0;
  float bias = max(0.005 * (1.0 - dot(N, L)), 0.001);
  vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
  float shadow = 0.0;
  for (int x = -1; x <= 1; ++x)
    for (int y = -1; y <= 1; ++y)
      shadow += (projCoords.z - bias > texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r ? 1.0 : 0.0);
  return shadow / 13.0;
}

float ShadowCalculationPoint(int idx, vec3 fragPos) {
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

  FragColor = vec4(result, (alphaMode == 2 ? baseSample.a : 1.0) * alpha);
}