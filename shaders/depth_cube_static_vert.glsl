#version 420 core

layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 shadowMatrices[15];

void main() {
	vec4 worldPos = model * vec4(aPos, 1.0);
	gl_Position = shadowMatrices[gl_InstanceID] * worldPos;
}