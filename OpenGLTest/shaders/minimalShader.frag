//uniform sampler2D qt_Texture0;
//varying vec4 qt_TexCoord0;

//void main(void)
//{
//    gl_FragColor = texture2D(qt_Texture0, qt_TexCoord0.st);
//}

//varying vec4 diffuse,ambientGlobal, ambient, ecPos;
//varying vec3 normal,halfVector;

//void main()
//{
//    vec3 n,halfV,viewV,lightDir;
//    float NdotL,NdotHV;
//    vec4 color = ambientGlobal;
//    float att, dist;

//    /* a fragment shader can't write a verying variable, hence we need
//    a new variable to store the normalized interpolated normal */
//    n = normalize(normal);

//    // Compute the ligt direction
//    lightDir = vec3(gl_LightSource[0].position-ecPos);

//    /* compute the distance to the light source to a varying variable*/
//    dist = length(lightDir);


//    /* compute the dot product between normal and ldir */
//    NdotL = max(dot(n,normalize(lightDir)),0.0);

//    if (NdotL > 0.0) {

//        att = 1.0 / (gl_LightSource[0].constantAttenuation +
//                gl_LightSource[0].linearAttenuation * dist +
//                gl_LightSource[0].quadraticAttenuation * dist * dist);
//        color += att * (diffuse * NdotL + ambient);


//        halfV = normalize(halfVector);
//        NdotHV = max(dot(n,halfV),0.0);
//        color += att * gl_FrontMaterial.specular * gl_LightSource[0].specular * pow(NdotHV,gl_FrontMaterial.shininess);
//    }

//    gl_FragColor = texture2D(qt_Texture0, qt_TexCoord0.st) * color;
//}

//precision mediump float;

//uniform sampler2D sampler2d;

//varying float v_Dot;
////varying vec2 v_texCoord;

//void main()
//{
//    //vec2 texCoord = vec2(v_texCoord.s, 1.0 - v_texCoord.t);
//    //vec4 color = texture2D(sampler2d, texCoord);
//    //color += vec4(0.1, 0.1, 0.1, 1);
//    //gl_FragColor = vec4(color.xyz * v_Dot, color.a);
//    gl_FragColor = v_Dot+vec4(0.1,0.2,0.1,0.5);
//}


out varying vec4 FragColor;
in vec3 gFacetNormal;
in vec3 gTriDistance;
in vec3 gPatchDistance;
in float gPrimitive;
uniform vec3 LightPosition;
uniform vec3 DiffuseMaterial;
uniform vec3 AmbientMaterial;

float amplify(float d, float scale, float offset)
{
    d = scale * d + offset;
    d = clamp(d, 0, 1);
    d = 1 - exp2(-2*d*d);
    return d;
}

void main()
{
    vec3 N = normalize(gFacetNormal);
    vec3 L = LightPosition;
    float df = abs(dot(N, L));
    vec3 color = AmbientMaterial + df * DiffuseMaterial;

    float d1 = min(min(gTriDistance.x, gTriDistance.y), gTriDistance.z);
    float d2 = min(min(gPatchDistance.x, gPatchDistance.y), gPatchDistance.z);
    color = amplify(d1, 40, -0.5) * amplify(d2, 60, -0.5) * color;

    FragColor = vec4(color, 1.0);
}
