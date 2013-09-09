#version 400

layout( triangles ) in;
layout( triangle_strip, max_vertices = 3 ) out;

in worldVertex {
    vec4 worldPosition;
    //vec3 worldNormal;
    vec4 position;
    //vec3 normal;
} In[];

out Vertex {
    noperspective vec3 edgeDistance;
    vec4 worldPosition;
    vec3 worldNormal;
	vec4 position;
	vec3 normal;
} output;

uniform mat4 viewportMatrix;

vec3 calcNormal(vec4 p1, vec4 p2, vec4 p3)
{
    vec3 u = p1.xyz - p2.xyz;
    vec3 v = p3.xyz - p2.xyz;
    return normalize(cross(u, v));
	//return vec3(0f,1f,0.0f);
}

void main()
{
    // Transform each vertex into viewport space
    vec2 p0 = vec2( viewportMatrix * ( gl_in[0].gl_Position / gl_in[0].gl_Position.w ) );
    vec2 p1 = vec2( viewportMatrix * ( gl_in[1].gl_Position / gl_in[1].gl_Position.w ) );
    vec2 p2 = vec2( viewportMatrix * ( gl_in[2].gl_Position / gl_in[2].gl_Position.w ) );

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

	//vec3 	norm1 = calcNormal(gl_in[0].gl_Position,gl_in[1].gl_Position,gl_in[2].gl_Position);
			//norm2 = calcNormal(gl_in[i1].gl_Position,gl_in[i3].gl_Position,pos2),
			//norm3 = norm;
	vec3 	normWorld = calcNormal(In[0].worldPosition,In[1].worldPosition,In[2].worldPosition);
	vec3 	norm = calcNormal(In[0].position,In[1].position,In[2].position);
    // Vertex 0 (a)
    output.edgeDistance = vec3( ha, 0, 0 );
    output.worldPosition = In[0].worldPosition;
    output.worldNormal = normWorld;
    output.position = In[0].position;
    output.normal = norm;
    //Out.texCoords = In[0].texCoords;
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    // Vertex 1 (b)
    output.edgeDistance = vec3( 0, hb, 0 );
    output.worldPosition = In[1].worldPosition;
    output.worldNormal = normWorld;
    output.position = In[1].position;
    output.normal = norm;
    //Out.texCoords = In[1].texCoords;
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();

    // Vertex 2 (c)
    output.edgeDistance = vec3( 0, 0, hc );
    output.worldPosition = In[2].worldPosition;
    output.worldNormal = normWorld;
    output.position = In[2].position;
    output.normal = norm;
    //Out.texCoords = In[2].texCoords;
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();

    // Finish the primitive off
    EndPrimitive();
}

