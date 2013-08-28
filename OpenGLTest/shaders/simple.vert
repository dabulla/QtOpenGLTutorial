//in vec4 Position;
//varying out vec3 vPosition;

//attribute vec4 Position;
//uniform mat4 uModelViewProjectionMatrix;

//void main()
//{
//    gl_Position = uModelViewProjectionMatrix * Position;
//}

#version 330
#pragma debug(on)

layout(location = 0) in vec3 VertexPosition;
//layout(location = 1) in vec3 VertexNormal;

//layout(location = 2) in vec2 TexCoord0;
//layout(location = 3) in vec2 TexCoord1;


//out VertexData {
	//vec2 TextureUV0;
	//vec2 TextureUV1;
	//vec3 Normal;
	//vec4 Position;
//} oput;

uniform vec3 lightSource;

uniform	mat4x4 uNormalMatrix;
uniform	mat4x4 uWorldMatrix;
uniform	mat4x4 uModelViewMatrix;
uniform	mat4x4 uModelViewProjectionMatrix;


void main() {
	//oput.TextureUV0 = vec2(TexCoord0.x, 1f-TexCoord0.y);
	//oput.TextureUV1 = vec2(TexCoord0.x, 1f-TexCoord0.y);
	//oput.Normal = VertexNormal;
	//oput.Position = WorldMatrix*vec4(VertexPosition, 1f);
	gl_Position = uModelViewProjectionMatrix*vec4(VertexPosition, 1f);
}