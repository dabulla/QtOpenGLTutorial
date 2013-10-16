#version 400
#pragma debug(on)

in vec3 in_Position;
in vec3 in_Normal;
in vec2 in_TexCoords;
in vec3 in_Tangent;
in vec3 in_Bitangent;

subroutine vec4 ShaderModelType();
subroutine uniform ShaderModelType shaderModel;

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

uniform	mat3x3 NormalMatrix;
uniform	mat3x3 ModelNormalMatrix;
uniform	mat4x4 ModelMatrix;
uniform	mat4x4 ModelViewMatrix;
uniform	mat4x4 ModelViewProjectionMatrix;

subroutine( ShaderModelType )
vec4 plain() {
	vec4 pos = vec4(in_Position, 1.0f);
	output.worldPosition = ModelMatrix*pos;
	output.worldNormal = ModelNormalMatrix*in_Normal;
	output.position = ModelViewMatrix*pos;
	output.normal = NormalMatrix*in_Normal;
    output.texCoords = in_TexCoords;
    output.tangent = NormalMatrix*in_Tangent;
    output.bitangent = NormalMatrix*in_Bitangent;

	return ModelViewProjectionMatrix*pos;
}

void main() {
	gl_Position = shaderModel();
}