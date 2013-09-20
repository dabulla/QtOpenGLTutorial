#version 400

layout(location=0) in vec4 in_Position;
layout(location=1) in vec4 in_Color;
out vec4 ex_Color;

uniform mat4x4 ModelViewProjectionMatrix;
void main(void)
{
	gl_Position = ModelViewProjectionMatrix * normalize(in_Position);
	vec4 col = normalize(in_Position)*4.0;
	//ex_Color = vec4(abs(col).x,0.0f,0.0f,1.0f);
	ex_Color = vec4(abs(col.x), abs(col.y), abs(col.z), 1.0f);
}