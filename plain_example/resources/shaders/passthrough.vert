#version 400
#pragma debug(on)

layout(location=0) in vec3 in_Position;
layout(location=1) in vec3 in_Normal;

out rawVertex {
    vec3 position;
    vec3 normal;
} output;

void main()
{
    output.position = in_Position;
    output.normal = in_Normal;
}
