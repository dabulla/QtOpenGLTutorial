#version 400
#pragma debug(on)

//layout (points) in;
layout( triangles ) in;
layout (line_strip, max_vertices = 6) out;

//structure MUST have the same name as in the vertex shader (and the same attributes)
in rawVertex {
    vec3 position;
    vec3 normal;
    vec2 texCoords;
    vec3 tangent;
    vec3 bitangent;
} input[3];

//structure MUST have the same name as in the fragment shader (and the same attributes)
out Vertex {
    noperspective vec3 edgeDistance;
    vec4 worldPosition;
    vec3 worldNormal;
	vec4 position;
	vec3 normal;
	float alpha;
    vec2 texCoords;
    vec3 tangent;
    vec3 bitangent;
} output;

uniform mat4 viewportMatrix;

//Make sure to not use reserved words like "smooth"
uniform bool doSmooth;

uniform bool explosionDoFaceScale;
uniform float explosionFactor;

uniform bool doWiggle;
uniform float wiggleWavelen;
uniform float wiggleAmp;

uniform	mat3x3 NormalMatrix;
uniform	mat3x3 ModelNormalMatrix;
uniform	mat4x4 ModelMatrix;
uniform	mat4x4 ModelViewMatrix;
uniform	mat4x4 ModelViewProjectionMatrix;

uniform float time;

const float normal_scale = 0.5;

void main()
{
    vec4 v0 = ModelViewProjectionMatrix * vec4(input[0].position, 1.f);
	output.position = v0;
    gl_Position = output.position;
    EmitVertex();

    vec4 v1 = v0 + vec4(NormalMatrix*input[0].normal * normal_scale, 0);
	output.position = v1;
    gl_Position = output.position;
    EmitVertex();
	
    EndPrimitive();
	
	output.position = v0;
    gl_Position = output.position;
    EmitVertex();

    v1 = v0 + vec4(NormalMatrix*input[0].tangent * normal_scale, 0);
	output.position = v1;
    gl_Position = output.position;
    EmitVertex();
	
    EndPrimitive();
	
	output.position = v0;
    gl_Position = output.position;
    EmitVertex();

    v1 = v0 + vec4(NormalMatrix*input[0].bitangent * normal_scale, 0);
	output.position = v1;
    gl_Position = output.position;
    EmitVertex();

    EndPrimitive();
}