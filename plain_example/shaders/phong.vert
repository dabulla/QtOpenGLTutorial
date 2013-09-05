#version 400
#pragma debug(on)

layout (location = 0) in vec4 vertexPosition;

out VertexData {
    vec4 Position;
} Out;

void main()
{
    Out.position = vertexPosition;
}

uniform	mat4x4 NormalMatrix;
uniform	mat4x4 WorldMatrix;
uniform	mat4x4 ModelViewMatrix;
uniform	mat4x4 ModelViewProjectionMatrix;


void main() {
	oput.TextureUV0 = vec2(TexCoord0.x, 1f-TexCoord0.y);
	oput.TextureUV1 = vec2(TexCoord0.x, 1f-TexCoord0.y);
	oput.Normal = VertexNormal;
	oput.Position = WorldMatrix*vec4(VertexPosition, 1f);
	gl_Position = ModelViewProjectionMatrix*vec4(VertexPosition, 1f);
}