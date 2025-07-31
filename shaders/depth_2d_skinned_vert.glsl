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