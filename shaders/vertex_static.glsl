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