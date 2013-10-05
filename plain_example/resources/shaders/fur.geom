#version 400
#pragma debug(on)

layout( triangles ) in;
layout( triangle_strip, max_vertices = 3) out;
layout(invocations = 8) in;

in rawVertex {
    vec3 position;
    vec3 normal;
} input[3];

//structure MUST have the same name as in the fragment shader (and the same attributes)
out Vertex {
    noperspective vec3 edgeDistance;
    vec4 worldPosition;
    vec3 worldNormal;
	vec4 position;
	vec3 normal;
	float alpha;
} output;

uniform mat4 viewportMatrix;

uniform int renderPasses;
uniform int renderPass;

//uniform int furLevels;
uniform float explosionFactor;

uniform	mat3x3 NormalMatrix;
uniform	mat3x3 ModelNormalMatrix;
uniform	mat4x4 ModelMatrix;
uniform	mat4x4 ModelViewMatrix;
uniform	mat4x4 ModelViewProjectionMatrix;

void main()
{
//	for (int currentLevel = 0; currentLevel < furLevels; currentLevel++)
//	{
		for (int i = 0; i < gl_in.length(); i++)
		{
			vec4 transformedPosition = ModelViewProjectionMatrix*vec4(input[i].position-input[i].normal*explosionFactor*float(renderPass), 1.0f);
//			vec4 transformedPosition = ModelViewProjectionMatrix*vec4(input[i].position, 1.0f);
			output.worldPosition = ModelMatrix*vec4(input[i].position, 1.0f);
			output.worldNormal = ModelNormalMatrix*input[i].normal;
			output.position = ModelViewMatrix*vec4(input[i].position, 1.0f);
			output.normal = NormalMatrix*input[i].normal;
			output.alpha = 1.f-float(renderPass+1)/float(renderPasses+1);
			gl_Position = transformedPosition;
			EmitVertex();
		}
		EndPrimitive();
//	}

}