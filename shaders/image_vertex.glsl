#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTex;

out vec2 TexCoord;

uniform vec2 position;
uniform vec2 scale;
uniform float rotation;
uniform float aspect_ratio;

void main()
{
    // Passo 1: trazer para centro ([-1,1] quad -> [-0.5, 0.5])
    vec2 centered = aPos * 0.5;

    // Passo 2: aplicar rotação
    float rad = radians(rotation);
    mat2 rot = mat2(cos(rad), -sin(rad),
                    sin(rad),  cos(rad));
    vec2 rotated = rot * centered;

    // Passo 3: corrigir aspect ratio (depois da rotação!)
    rotated.y *= aspect_ratio;

    // Passo 4: aplicar escala
    vec2 scaled = rotated * scale;

    // Passo 5: aplicar posição final
    vec2 translated = scaled + position;

    gl_Position = vec4(translated, 0.0, 1.0);
    TexCoord = aTex;
}