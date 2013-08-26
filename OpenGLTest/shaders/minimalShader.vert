////attribute vec4 qt_Vertex;
////attribute vec4 qt_MultiTexCoord0;
////uniform mat4 qt_ModelViewProjectionMatrix;
////varying vec4 qt_TexCoord0;

////varying vec4 diffuse,ambientGlobal,ambient, ecPos;
////varying vec3 normal,halfVector;

////void main()
////{
////    vec3 aux;

////    /* first transform the normal into eye space and normalize the result */
////    normal = normalize(gl_NormalMatrix * gl_Normal);

////    /* compute the vertex position  in camera space. */
////    ecPos = gl_ModelViewMatrix * gl_Vertex;

////    /* Normalize the halfVector to pass it to the fragment shader */
////    halfVector = gl_LightSource[0].halfVector.xyz;

////    /* Compute the diffuse, ambient and globalAmbient terms */
////    diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
////    ambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
////    ambientGlobal = gl_LightModel.ambient * gl_FrontMaterial.ambient;

////    gl_Position = qt_ModelViewProjectionMatrix * qt_Vertex;
////    qt_TexCoord0 = qt_MultiTexCoord0;
////}

//uniform mat4 u_modelViewProjMatrix;
//uniform mat4 u_normalMatrix;

//uniform struct LightInfo
//{
//    vec4 position;  // Light position in eye coords.
//    vec3 intensity;
//    vec3 direction;  // Light position in eye coords.
//} light;

////attribute vec3 vNormal;
////attribute vec4 vTexCoord;
//attribute vec4 vPosition;

//varying float v_Dot;
////varying vec2 v_texCoord;

//void main()
//{
//    gl_Position = u_modelViewProjMatrix * vPosition;
//    //v_texCoord = vTexCoord.st;
//    vec4 transNormal = u_normalMatrix * vec4(gl_Normal, 1);
//    v_Dot = max(dot(transNormal.xyz, light.direction), 0.0);
//}

in vec4 Position;
out vec3 vPosition;

void main()
{
    vPosition = Position.xyz;
}
