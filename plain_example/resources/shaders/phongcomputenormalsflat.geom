#version 400
#pragma debug(on)

layout( triangles ) in;
layout( triangle_strip, max_vertices = 3 ) out;

//structure MUST have the same name as in the vertex shader (and the same attributes)
in rawVertex {
    vec3 position;
    vec3 normal;
    vec2 texCoords;
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
} output;

uniform mat4 viewportMatrix;

//Make sure to not use reserved words like "smooth"
uniform bool doSmooth;

uniform bool explosionDoFaceScale;
uniform float explosionFactor;

uniform	mat3x3 NormalMatrix;
uniform	mat3x3 ModelNormalMatrix;
uniform	mat4x4 ModelMatrix;
uniform	mat4x4 ModelViewMatrix;
uniform	mat4x4 ModelViewProjectionMatrix;

vec3 calcNormal(vec3 p1, vec3 p2, vec3 p3)
{
    vec3 u = p1.xyz - p2.xyz;
    vec3 v = p3.xyz - p2.xyz;
    return normalize(cross(u, v));
}

void main()
{
	// Face Normal. Used for Flat shading and explosion of faces.
	vec3 faceNormal = calcNormal(input[0].position, input[1].position, input[2].position);
	
	vec3 normal[3];
	if(doSmooth) {
		normal[0] = normalize(input[0].normal);
		normal[1] = normalize(input[1].normal);
		normal[2] = normalize(input[2].normal);
	} else {
		normal[0] = faceNormal;
		normal[1] = faceNormal;
		normal[2] = faceNormal;
	}
	
	vec3 scaledNormal[3];
	if(explosionDoFaceScale)
	{
		scaledNormal[0] = normalize(input[0].normal);
		scaledNormal[1] = normalize(input[1].normal);
		scaledNormal[2] = normalize(input[2].normal);
	} else {
		scaledNormal[0] = faceNormal;
		scaledNormal[1] = faceNormal;
		scaledNormal[2] = faceNormal;
	}
	scaledNormal[0] *= explosionFactor;
	scaledNormal[1] *= explosionFactor;
	scaledNormal[2] *= explosionFactor;
	
	vec4 transformedPosition[3];
	transformedPosition[0] = ModelViewProjectionMatrix*vec4(input[0].position-scaledNormal[0], 1.0f);
	transformedPosition[1] = ModelViewProjectionMatrix*vec4(input[1].position-scaledNormal[1], 1.0f);
	transformedPosition[2] = ModelViewProjectionMatrix*vec4(input[2].position-scaledNormal[2], 1.0f);
	
    // Transform each vertex into viewport space
    vec2 p0 = vec2( viewportMatrix * ( transformedPosition[0] / transformedPosition[0].w ) );
    vec2 p1 = vec2( viewportMatrix * ( transformedPosition[1] / transformedPosition[1].w ) );
    vec2 p2 = vec2( viewportMatrix * ( transformedPosition[2] / transformedPosition[2].w ) );

    // Calculate lengths of 3 edges of triangle
    float a = length( p1 - p2 );
    float b = length( p2 - p0 );
    float c = length( p1 - p0 );

    // Calculate internal angles using the cosine rule
    float alpha = acos( ( b * b + c * c - a * a ) / ( 2.0 * b * c ) );
    float beta = acos( ( a * a + c * c - b * b ) / ( 2.0 * a * c ) );

    // Calculate the perpendicular distance of each vertex from the opposing edge
    float ha = abs( c * sin( beta ) );
    float hb = abs( c * sin( alpha ) );
    float hc = abs( b * sin( alpha ) );

    // Now add this perpendicular distance as a per-vertex property in addition to
    // the position calculated in the vertex shader.

	
	output.edgeDistance = vec3( ha, 0.f, 0.f );
	output.worldPosition = ModelMatrix*vec4(input[0].position-scaledNormal[0], 1.0f);
	output.worldNormal = ModelNormalMatrix*normal[0];
	output.position = ModelViewMatrix*vec4(input[0].position-scaledNormal[0], 1.0f);
	output.normal = NormalMatrix*normal[0];
	output.alpha = 1.0f;
    output.texCoords = input[0].texCoords;
	gl_Position = transformedPosition[0];
	EmitVertex();
	output.edgeDistance = vec3( 0.f, hb, 0.f );
	output.worldPosition = ModelMatrix*vec4(input[1].position-scaledNormal[1], 1.0f);
	output.worldNormal = ModelNormalMatrix*normal[1];
	output.position = ModelViewMatrix*vec4(input[1].position-scaledNormal[1], 1.0f);
	output.normal = NormalMatrix*normal[1];
	output.alpha = 1.0f;
    output.texCoords = input[1].texCoords;
	gl_Position = transformedPosition[1];
	EmitVertex();
	output.edgeDistance = vec3( 0.f, 0.f, hc );
	output.worldPosition = ModelMatrix*vec4(input[2].position-scaledNormal[2], 1.0f);
	output.worldNormal = ModelNormalMatrix*normal[2];
	output.position = ModelViewMatrix*vec4(input[2].position-scaledNormal[2], 1.0f);
	output.normal = NormalMatrix*normal[2];
	output.alpha = 1.0f;
    output.texCoords = input[2].texCoords;
	gl_Position = transformedPosition[2];
	EmitVertex();
    // Finish the primitive off
    EndPrimitive();
}