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
	vec3 lightPos = vec3(ModelViewMatrix*light.position);
    // Some useful vectors
    vec3 s = normalize( light.position.xyz - input.worldPosition.xyz );
    vec3 v = normalize( -input.position.xyz );
    vec3 n = normalize( input.normal );
    vec3 r = reflect( -s, n );

    // Calculate the ambient contribution
    vec3 ambient = light.intensity * material.Ka;

    // Calculate the diffuse contribution
    float sDotN = max( dot( s, n ), 0.0 );
    vec3 diffuse = light.intensity * material.Kd * sDotN;

    // Sum the ambient and diffuse contributions
    ambientAndDiff = ambient + diffuse;

    // Calculate the specular highlight component
    spec = vec3( 0.0 );
    if ( sDotN > 0.0 )
    {
        spec = light.intensity * material.Ks *
               pow( max( dot( r, v ), 0.0 ), material.shininess );
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