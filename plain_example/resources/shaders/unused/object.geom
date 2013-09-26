#version 330
layout(triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VertexData {
	//vec2 TextureUV0;
	//vec2 TextureUV1;
	//vec3 Normal;
	vec4 Position;
} vertexData[];

out VertexData2 {
	//vec2 TextureUV0;
	//vec2 TextureUV1;
	vec3 Normal;
	vec4 Position;
} oput;

//in int gl_PrimitiveIDIn;

uniform	mat4x4 ModelViewProjectionMatrix;

uniform float anim;

vec3 calcNormal(vec4 p1, vec4 p2, vec4 p3)
{
    vec3 u = p1.xyz - p2.xyz;
    vec3 v = p3.xyz - p2.xyz;
    return normalize(cross(u, v));
	//return vec3(0f,1f,0.0f);
}

void main()
{
/*	for(int i=0; i<gl_in.length(); i++) {
		gl_Position = gl_in[i].gl_Position;
		oput.TextureUV0 = vertexData[i].TextureUV0;
		oput.TextureUV1 = vertexData[i].TextureUV1;
		oput.Normal = vertexData[i].Normal;
		oput.Position =  vertexData[i].Position;
		EmitVertex();
		
	}*/
		gl_Position = gl_in[0].gl_Position;
		//oput.TextureUV0 = vertexData[0].TextureUV0;
		//oput.TextureUV1 = vertexData[0].TextureUV1;
		oput.Normal = vertexData[0].Normal;
		oput.Position =  vertexData[0].Position;
		EmitVertex();
		gl_Position = gl_in[1].gl_Position;
		//oput.TextureUV0 = vertexData[1].TextureUV0;
		//oput.TextureUV1 = vertexData[1].TextureUV1;
		oput.Normal = vertexData[1].Normal;
		oput.Position =  vertexData[1].Position;
		EmitVertex();
		gl_Position = gl_in[2].gl_Position;
		//oput.TextureUV0 = vertexData[2].TextureUV0;
		//oput.TextureUV1 = vertexData[2].TextureUV1;
		oput.Normal = vertexData[2].Normal;
		oput.Position =  vertexData[2].Position;
		EmitVertex();
	EndPrimitive();
}
