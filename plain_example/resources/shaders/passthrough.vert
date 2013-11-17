#version 400
#pragma debug(on)

//layout-locations are a way to bind variables without looking up the names
layout(location=0) in vec3 in_Position;
layout(location=1) in vec3 in_Normal;
layout(location=2) in vec2 in_TexCoords;
layout(location=3) in vec3 in_Tangent;
layout(location=4) in vec3 in_Bitangent;

uniform float texFactor;

out rawVertex {
    vec3 position;
    vec3 normal;
    vec2 texCoords;
    vec3 tangent;
    vec3 bitangent;
} output;

void main()
{
    output.position = in_Position;
    output.normal = in_Normal;
    output.texCoords = in_TexCoords*texFactor;//in_Position.xy;//
    output.tangent = in_Tangent;//in_Position.xy;//
    output.bitangent = in_Bitangent;//in_Position.xy;//
}
