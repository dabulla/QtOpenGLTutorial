#version 400

//layout (location = 0) in vec2 vertexPosition;

//out terrainVertex {
//    vec2 position;
//} Out;

//void main()
//{
//    Out.position = vertexPosition;
//}

//#version 330
#pragma debug(on)

layout(location = 0) in vec3 vertexPosition;
//layout(location = 1) in vec3 VertexNormal;

//layout(location = 2) in vec2 TexCoord0;
//layout(location = 3) in vec2 TexCoord1;


out VertexData {
	//vec2 TextureUV0;
	//vec2 TextureUV1;
	//vec3 Normal;
	vec4 Position;
} oput;

uniform vec3 lightSource;

//uniform	mat4x4 NormalMatrix;
//uniform	mat4x4 WorldMatrix;
//uniform	mat4x4 ModelViewMatrix;
//uniform	mat4x4 ModelViewProjectionMatrix;

uniform mat4 modelMatrix;
uniform mat4 modelViewMatrix;
uniform mat3 worldNormalMatrix;
uniform mat3 normalMatrix;
uniform mat4 mvp;


void main() {
	//oput.TextureUV0 = vec2(TexCoord0.x, 1f-TexCoord0.y);
	//oput.TextureUV1 = vec2(TexCoord0.x, 1f-TexCoord0.y);
	//oput.Normal = VertexNormal;
	oput.Position = modelMatrix*vec4(vertexPosition, 1f);
	gl_Position = mvp*vec4(vertexPosition, 1f);
}