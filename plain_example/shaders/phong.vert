#version 400
#pragma debug(on)

//layout (location = 0) in vec4 vertexPosition;
layout(location=0) in vec4 in_Position;

out worldVertex {
    vec4 worldPosition;
    vec4 position;
} output;

uniform	mat4x4 NormalMatrix;
uniform	mat4x4 WorldMatrix;
uniform	mat4x4 ModelViewMatrix;
uniform	mat4x4 ModelViewProjectionMatrix;

//void main()
//{
//    Out.position = vertexPosition;
//	gl_Position = ModelViewProjectionMatrix*vertexPosition
//}

void main() {
	output.worldPosition = WorldMatrix*in_Position;
	output.position = ModelViewMatrix*in_Position;
	gl_Position = ModelViewProjectionMatrix*in_Position;
}