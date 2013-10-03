#version 330
#pragma debug(on)

uniform struct LightInfo
{
    vec4 position;  // Light position in eye coords.
    vec3 intensity;
} light;

in VertexData2 {
	//vec2 TextureUV0;
	//vec2 TextureUV1;
	vec3 Normal;
	vec4 Position;
} iput;

layout (location = 0) out vec4 FragColor;


uniform vec3 lightSource;
uniform mat4x4 NormalMatrix;
uniform	mat4x4 WorldMatrix;
uniform mat4x4 ModelViewMatrix;
uniform mat4x4 ModelViewProjectionMatrix;

uniform sampler2D ColorTexture;
uniform sampler2D NormalTexture;
uniform sampler2D SpecTexture;

float falloff(float start, float end, float val) {
	return 1f-(cos(min(max((val-start)/(end-start),0f),1f)*3.1415)*0.5f+0.5f);
}
void main()
{
     vec4 texel0, texel1, resultColor, fragPos;
     //-------------------
     //texel0 = texture2D(ColorTexture, iput.TextureUV0);
     //texel1 = texture2D(NormalTexture, iput.TextureUV1);
     //-------------------
     //resultColor = texel0;
	 vec4 diff = light.position-iput.Position;
	 //vec3 texelnorm = vec3(texel1*2f-1f);
	 //float diffuse_value = dot(normalize(iput.Normal)*texelnorm, normalize(diff).xyz);
	
	 //FragColor = resultColor*diffuse_value;
	 FragColor = vec4(1f,0.2f,0.2f,0.5f);
}