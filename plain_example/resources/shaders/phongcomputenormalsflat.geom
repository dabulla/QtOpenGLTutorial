#version 400
#pragma debug(on)

layout( triangles ) in;
layout( triangle_strip, max_vertices = 3 ) out;

//structure MUST have the same name as in the vertex shader (and the same attributes)
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

uniform float scale;

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
	vec4 transformedPosition[3];
	transformedPosition[0] = ModelViewProjectionMatrix*vec4(input[0].position-input[0].normal*scale*0.01, 1.0f);
	transformedPosition[1] = ModelViewProjectionMatrix*vec4(input[1].position-input[1].normal*scale*0.01, 1.0f);
	transformedPosition[2] = ModelViewProjectionMatrix*vec4(input[2].position-input[2].normal*scale*0.01, 1.0f);
	
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

	vec3 normWorld = calcNormal(input[0].position, input[1].position, input[2].position);
/*
	for(int i = 0; i < 3; i++) {
		output.edgeDistance = vec3( ha*float(i==0), hb*float(i==1), hc*float(i==2) );
		output.worldPosition = ModelMatrix*vec4(input[i].position-input[i].normal*scale*0.01, 1.0f);
		output.worldNormal = ModelNormalMatrix*normWorld;
		output.position = ModelViewMatrix*vec4(input[i].position-input[i].normal*scale*0.01, 10f);
		output.normal = NormalMatrix*normWorld;
		gl_Position = ModelViewProjectionMatrix*vec4(input[i].position-input[i].normal*scale*0.01, 1.0f);
		EmitVertex();
	}
*/
		output.edgeDistance = vec3( ha, 0.f, 0.f );
		output.worldPosition = ModelMatrix*vec4(input[0].position-input[0].normal*scale*0.01, 1.0f);
		output.worldNormal = ModelNormalMatrix*normWorld;
		output.position = ModelViewMatrix*vec4(input[0].position-input[0].normal*scale*0.01, 10f);
		output.normal = NormalMatrix*normWorld;
		gl_Position = transformedPosition[0];
		EmitVertex();
		output.edgeDistance = vec3( 0.f, hb, 0.f );
		output.worldPosition = ModelMatrix*vec4(input[1].position-input[1].normal*scale*0.01, 1.0f);
		output.worldNormal = ModelNormalMatrix*normWorld;
		output.position = ModelViewMatrix*vec4(input[1].position-input[1].normal*scale*0.01, 10f);
		output.normal = NormalMatrix*normWorld;
		gl_Position = transformedPosition[1];
		EmitVertex();
		output.edgeDistance = vec3( 0.f, 0.f, hc );
		output.worldPosition = ModelMatrix*vec4(input[2].position-input[2].normal*scale*0.01, 1.0f);
		output.worldNormal = ModelNormalMatrix*normWorld;
		output.position = ModelViewMatrix*vec4(input[2].position-input[2].normal*scale*0.01, 10f);
		output.normal = NormalMatrix*normWorld;
		gl_Position = transformedPosition[2];
		EmitVertex();
    // Finish the primitive off
    EndPrimitive();
}