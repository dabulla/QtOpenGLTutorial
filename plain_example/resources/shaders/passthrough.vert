#version 400
#pragma debug(on)

layout(location=0) in vec3 in_Position;
layout(location=1) in vec3 in_Normal;
layout(location=2) in vec2 in_TexCoords;

out rawVertex {
    vec3 position;
    vec3 normal;
    vec2 texCoords;
} output;

void main()
{
    output.position = in_Position;
    output.normal = in_Normal;
    output.texCoords = in_TexCoords;//in_Position.xy;//
}
