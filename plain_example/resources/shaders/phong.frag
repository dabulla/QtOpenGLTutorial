#version 420
#pragma debug(on)

subroutine vec4 ShaderModelType();
subroutine uniform ShaderModelType shaderModel;

in Vertex {
    noperspective vec3 edgeDistance;
    vec4 worldPosition;
    vec3 worldNormal;
	vec4 position;
	vec3 normal;
	float alpha;
    vec2 texCoords;
    vec3 tangent;
    vec3 bitangent;
} input;

layout (location = 0) out vec4 fragColor;

uniform struct LightInfo
{
    vec3 position;  // Light position in world coords.
    vec3 intensity;
} light;

uniform struct MaterialInfo
{
    vec3 Ka;            // Ambient reflectivity
    vec3 Kd;            // Diffuse reflectivity
    vec3 Ks;            // Specular reflectivity
    float shininess;    // Specular shininess exponent
} material;

uniform struct LineInfo {
  float width;
  vec3 color;
} line;

uniform bool doWireframe;
uniform float wireframeAlpha;

uniform float texFactor;

uniform bool useSpecularMap;
uniform float specularTextureFactor;

uniform float alpha;
uniform float bumpFactor;

uniform sampler2D diffuseTexture;
uniform sampler2D heightTexture;
uniform sampler2D normalTexture;
uniform sampler2D specularTexture;

uniform mat3x3 NormalMatrix;
uniform mat4x4 ViewMatrix;
uniform	mat4x4 ModelMatrix;
uniform mat4x4 ModelViewMatrix;
uniform mat4x4 ModelViewProjectionMatrix;

vec4 getTextureColorProjected(sampler2D samp)
{
	vec4 texZ = texture( samp, input.worldPosition.xy*texFactor );
	vec4 texX = texture( samp, input.worldPosition.zy*texFactor );
	vec4 texY = texture( samp, input.worldPosition.xz*texFactor );
	float mixZ = smoothstep( 0.02, 0.12, abs(dot(input.worldNormal, vec3(0.0f,0.0f,1.0f))));
	float mixX = smoothstep( 0.02, 0.12, abs(dot(input.worldNormal, vec3(1.0f,0.0f,0.0f))));
	float mixY = smoothstep( 0.02, 0.12, abs(dot(input.worldNormal, vec3(0.0f,1.0f,0.0f))));
	return texZ*mixZ+texY*mixY+texX*mixX;
}

float getSpecular()
{
	float spec = 1.f;
	if(useSpecularMap)
	{
		vec4 texSpec = texture(specularTexture, input.texCoords);
		if(specularTextureFactor<0)
		{
			spec*=1-texSpec.x*-specularTextureFactor;
		} else {
			spec*=1-(1-texSpec.x)*specularTextureFactor;
		}
	}
	return spec;
}

float getSpecularProjected()
{
	float spec = 1.f;
	if(useSpecularMap)
	{
		vec4 texSpec = getTextureColorProjected(specularTexture);
		if(specularTextureFactor>0)
		{
			spec*=1-texSpec.x*specularTextureFactor;
		} else {
			spec*=1-(1-texSpec.x)*-specularTextureFactor;
		}
	}
	return spec;
}

vec3 toTangentSpace(vec3 v)
{
	vec3 result;
	result.x = dot(v, normalize(input.tangent)*bumpFactor);
	result.y = dot(v, normalize(input.bitangent)*bumpFactor);
	result.z = dot(v, normalize(input.normal));
	return normalize(result);
}

void bumpyPhongModel(out vec3 ambientAndDiff, out vec3 spec, vec3 normalOffset)
{
	vec4 lightPosEye = ViewMatrix*vec4(light.position, 1.0f);
    vec3 lightDir = toTangentSpace( normalize(lightPosEye.xyz - input.position.xyz ));
    vec3 viewDir = toTangentSpace( normalize(-input.position.xyz ));
    vec3 normal = normalOffset;
	
    vec3 reflectDir  = reflect( -lightDir,  normal );

    // Calculate the ambient contribution
    vec3 ambient = light.intensity * material.Ka;

    // Calculate the diffuse contribution
    float lambertian = max( dot( lightDir, normal ), 0.0 );
    vec3 diffuse = light.intensity * material.Kd * lambertian;

    // Sum the ambient and diffuse contributions
    ambientAndDiff = ambient + diffuse;
    // Calculate the specular highlight component
    spec = vec3( 0.0 );
    //if ( lambertian > 0.0 )
    {
		spec = light.intensity * material.Ks *
           pow( max( dot( reflectDir , viewDir ), 0.0 ), exp(material.shininess) );
    }
}
void phongModel(out vec3 ambientAndDiff, out vec3 spec)
{
	vec4 lightPosEye = ViewMatrix*vec4(light.position, 1.0f);
    vec3 lightDir = normalize( lightPosEye.xyz - input.position.xyz );
    vec3 viewDir = normalize( -input.position.xyz );
    vec3 normal = normalize( input.normal );
    vec3 reflectDir  = reflect( -lightDir, normal );

    // Calculate the ambient contribution
    vec3 ambient = light.intensity * material.Ka;

    // Calculate the diffuse contribution
    float lambertian = max( dot( lightDir, normal ), 0.0 );
    vec3 diffuse = light.intensity * material.Kd * lambertian;

    // Sum the ambient and diffuse contributions
    ambientAndDiff = ambient + diffuse;

    // Calculate the specular highlight component
    spec = vec3( 0.0 );
    if ( lambertian > 0.0 )
    {
		spec = light.intensity * material.Ks *
           pow( max( dot( reflectDir , viewDir ), 0.0 ), exp(material.shininess) );
    }
}

subroutine( ShaderModelType )
vec4 wireFrame()
{
    // Find the smallest distance between the fragment and a triangle edge
    float d = min( input.edgeDistance.x, input.edgeDistance.y );
    d = min( d, input.edgeDistance.z );

    // Blend between line color and triangle color
    float mixVal;
    if ( d < line.width - 0.5 )
    {
        mixVal = 1.0;
    }
    else if ( d > line.width + 0.5 )
    {
        mixVal = 0.0;
    }
    else
    {
        float x = d - ( line.width - 1.0 );
        mixVal = exp2( -2.0 * ( x * x ) );
    }
    vec4 c = mix( vec4(0.0f, 0.0f, 0.0f, alpha), vec4(line.color, 1.0), mixVal );
    return c;
}

subroutine( ShaderModelType )
vec4 plainPhong()
{
    vec3 ambientAndDiff, spec;
	
    phongModel( ambientAndDiff, spec);
	
	return vec4(ambientAndDiff + spec, alpha);
}

subroutine( ShaderModelType )
vec4 texturePhong()
{
    vec3 ambientAndDiff, spec;
	phongModel( ambientAndDiff, spec);
	
	vec4 tex = texture(diffuseTexture, input.texCoords);
	spec *= getSpecular();
	return vec4((ambientAndDiff + spec) * tex.xyz, alpha*input.alpha);
}

subroutine( ShaderModelType )
vec4 bumpyPhong()
{
    vec3 ambientAndDiff, spec;
    vec3 normalMap = normalize((texture2D(normalTexture, input.texCoords).xyz * 2.0) - 1.0);
    bumpyPhongModel( ambientAndDiff, spec,  normalMap);
	
	vec4 tex = texture(diffuseTexture, input.texCoords);
	spec *= getSpecular();
	return vec4((ambientAndDiff + spec) * tex.xyz, alpha*input.alpha);
}

subroutine( ShaderModelType )
vec4 textureProjectedPhong()
{
    vec3 ambientAndDiff, spec;
    phongModel( ambientAndDiff, spec);
	vec4 tex = getTextureColorProjected(diffuseTexture);
	spec *= getSpecularProjected();
	return vec4((ambientAndDiff) * tex.xyz + spec, alpha*input.alpha);
}

subroutine( ShaderModelType )
vec4 furPhong()
{
    vec3 ambientAndDiff, spec;
    phongModel( ambientAndDiff, spec);
	vec4 tex = getTextureColorProjected(diffuseTexture);
	vec4 transp = getTextureColorProjected(heightTexture);
	return vec4((ambientAndDiff + spec) * tex.xyz, alpha*input.alpha*transp.x);
}

subroutine( ShaderModelType )
vec4 showNormals()
{
	return vec4(input.worldNormal*3.0f, alpha*input.alpha);
}

subroutine( ShaderModelType )
vec4 showNormalsNormalmap()
{
	return vec4(toTangentSpace(texture2D(normalTexture, input.texCoords).xyz), alpha*input.alpha);
}

void main()
{
	fragColor = shaderModel();
	if(doWireframe)
	{
		fragColor += wireFrame()*wireframeAlpha;
	}
}