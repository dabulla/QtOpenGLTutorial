#version 420
#pragma debug(on)

in Vertex {
    //noperspective vec3 edgeDistance;
    vec4 worldPosition;
    vec3 worldNormal;
	vec4 position;
	vec3 normal;
} input;

layout (location = 0) out vec4 fragColor;

uniform struct LightInfo
{
    vec4 position;  // Light position in eye coords.
    vec3 intensity;
} light;

uniform struct MaterialInfo
{
    vec3 Ka;            // Ambient reflectivity
    vec3 Kd;            // Diffuse reflectivity
    vec3 Ks;            // Specular reflectivity
    float shininess;    // Specular shininess exponent
} material;

uniform mat3x3 NormalMatrix;
uniform	mat4x4 ModelMatrix;
uniform mat4x4 ModelViewMatrix;
uniform mat4x4 ModelViewProjectionMatrix;

void phongModel( out vec3 ambientAndDiff, out vec3 spec )
{
    vec3 lightDir = normalize( light.position.xyz + input.position.xyz );
    vec3 viewDir = normalize( input.position.xyz );
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
               pow( max( dot( reflectDir , viewDir ), 0.0 ), material.shininess );
    }
}

void main()
{
    vec3 ambientAndDiff, spec;
    phongModel( ambientAndDiff, spec );
    fragColor = vec4( ambientAndDiff, 1.0 ) + vec4( spec, 1.0 );
	//fragColor = vec4(input.worldNormal, 0.5f);
	//fragColor = vec4(1.f,0.5f,0.0f, 0.5f);
}