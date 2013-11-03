#version 400
#pragma debug(on)

layout( triangles ) in;
layout( triangle_strip, max_vertices = 3 ) out;

const float PI = 3.14159265359;

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
uniform float wiggleFrequence;
uniform float wiggleAmp;

uniform	mat3x3 NormalMatrix;
uniform	mat3x3 ModelNormalMatrix;
uniform	mat4x4 ModelMatrix;
uniform	mat4x4 ModelViewMatrix;
uniform	mat4x4 ModelViewProjectionMatrix;

uniform float time;

vec3 calcNormal(vec3 p1, vec3 p2, vec3 p3)
{
    vec3 u = p2.xyz - p1.xyz;
    vec3 v = p3.xyz - p2.xyz;
    return normalize(cross(u, v));
}

mat3 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat3(	oc * axis.x * axis.x + c,			oc * axis.x * axis.y - axis.z * s,	oc * axis.z * axis.x + axis.y * s,
						oc * axis.x * axis.y + axis.z * s,oc * axis.y * axis.y + c,			oc * axis.y * axis.z - axis.x * s,
						oc * axis.z * axis.x - axis.y * s,	oc * axis.y * axis.z + axis.x * s,oc * axis.z * axis.z + c);
}

void wiggle(inout vec4 position, inout vec3 normal, inout vec3 tangent, inout vec3 bitangent)
{
	float anim = (position.x+time*0.01f)*wiggleFrequence;
	float animDerivate = wiggleFrequence;
	float amp = sin(anim)*wiggleAmp*0.01f;
	float ampDerivate = cos(anim)*animDerivate*wiggleAmp*0.01f; // derivation of amp for normal
	float angle = atan(ampDerivate); // calculate angle from gradient
	
	// use the normal (without the x component) and the x-Axis to calculate an axis, to rotate the normal along
	vec3 rotationAxis = cross(vec3(0.f, normal.y, normal.z),vec3(1.f,0.f,0.f));
	mat3x3 rotY = rotationMatrix(rotationAxis, angle);
									
	position += vec4(normal*amp, 0.0f); // fourth component zero, because we add the vector to the position

	normal = rotY*normal;
	tangent = rotY*tangent;
	bitangent = rotY*bitangent;
}

void main()
{
	// Face Normal. Used for Flat shading and explosion of faces.
	vec3 faceNormal = calcNormal(input[0].position, input[1].position, input[2].position);
	
	//Smooth
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
	
	//Explode
	vec3 explosionOffset[3];
	if(explosionDoFaceScale)
	{
		explosionOffset[0] = normalize(input[0].normal);
		explosionOffset[1] = normalize(input[1].normal);
		explosionOffset[2] = normalize(input[2].normal);
	} else {
		explosionOffset[0] = faceNormal;
		explosionOffset[1] = faceNormal;
		explosionOffset[2] = faceNormal;
	}
	explosionOffset[0] *= explosionFactor;
	explosionOffset[1] *= explosionFactor;
	explosionOffset[2] *= explosionFactor;

	vec4 worldPosition[3];
	worldPosition[0] = vec4(input[0].position+explosionOffset[0], 1.f);
	worldPosition[1] = vec4(input[1].position+explosionOffset[1], 1.f);
	worldPosition[2] = vec4(input[2].position+explosionOffset[2], 1.f);
	
	vec3 tangent[3];
	vec3 bitangent[3];
	
	if(doWiggle)
	{
		//use the normal to wiggle and change both, the position and the normal.
		//wiggle(worldPosition[0], normal[0], tangent[0], bitangent[0]);
		//wiggle(worldPosition[1], normal[1], tangent[1], bitangent[1]);
		//wiggle(worldPosition[2], normal[2], tangent[2], bitangent[2]);
		wiggle(worldPosition[0], normal[0], input[0].tangent, input[0].bitangent);
		wiggle(worldPosition[1], normal[1], input[1].tangent, input[1].bitangent);
		wiggle(worldPosition[2], normal[2], input[2].tangent, input[2].bitangent);
	} else {
	
	}
	
	vec4 transformedPosition[3];
	transformedPosition[0] = ModelViewProjectionMatrix*worldPosition[0];
	transformedPosition[1] = ModelViewProjectionMatrix*worldPosition[1];
	transformedPosition[2] = ModelViewProjectionMatrix*worldPosition[2];

	//Nice Wireframe
	// has some advantages againt lines primitives: Backface culling, line thickness, pixel perfect lighting, ...
	
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

	vec3 edgeDistance[3];
	edgeDistance[0] = vec3( ha, 0.f, 0.f );
	edgeDistance[1] = vec3( 0.f, hb, 0.f );
	edgeDistance[2] = vec3( 0.f, 0.f, hc );
	
	for (int i=0; i<3; i++)
	{
		output.edgeDistance = edgeDistance[i];
		output.worldPosition = ModelMatrix*worldPosition[i];
		output.worldNormal = ModelNormalMatrix*normal[i];
		output.position = ModelViewMatrix*worldPosition[i];
		output.normal = NormalMatrix*normal[i];
		output.alpha = 1.0f;
		output.texCoords = input[i].texCoords;
		output.tangent = NormalMatrix*input[i].tangent;
		output.bitangent = NormalMatrix*input[i].bitangent;
		gl_Position = transformedPosition[i];
		EmitVertex();
	}
    // Finish the primitive off
    EndPrimitive();
}