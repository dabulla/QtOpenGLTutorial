#include "terraintessellationscene.h"
#include "camera.h"

#include <qdir.h>
#include <QFile>
#include <QImage>
#include <QGLWidget>
#include <QOpenGLContext>
#include <QOpenGLFunctions_4_3_Core>
#include <QQuickItem.h>
#include <qslider.h>
#include <qquickview.h>

#include "LoaderObj.h"

#define _USE_MATH_DEFINES
#include <math.h>

const float degToRad = float( M_PI / 180.0 );

TerrainTessellationScene::TerrainTessellationScene( QObject* parent )
    : AbstractScene( parent ),
      m_camera( new Camera( this ) ),
      m_v(),
      m_viewCenterFixed( false ),
      m_panAngle( 0.0f ),
      m_tiltAngle( 0.0f ),
      //m_patchBuffer( QOpenGLBuffer::VertexBuffer ),
      m_normalsBuffer( QOpenGLBuffer::VertexBuffer ),
      m_vertexBuffer( QOpenGLBuffer::VertexBuffer ),
      m_indexBuffer( QOpenGLBuffer::IndexBuffer ),
      m_screenSpaceError( 12.0f ),
      m_modelMatrix(),
      m_horizontalScale( 500.0f ),
      m_verticalScale( 20.0f ),
      m_sunTheta( 30.0f ),
      m_time( 0.0f ),
      m_metersToUnits( 0.05f ), // 500 units == 10 km => 0.05 units/m
      m_displayMode( SimpleWireFrame ),
      m_displayModeSubroutines( DisplayModeCount ),
      m_funcs( 0 )
{
    // Initialize the camera position and orientation
    m_camera->setPosition( QVector3D( -0.3f, 0.1f, 1.0f ) );
    m_camera->setViewCenter( QVector3D( 0.0f, 0.1f, 0.0f ) );
    m_camera->setUpVector( QVector3D( 0.0f, 1.0f, 0.0f ) );

    //m_displayModeNames << QStringLiteral( "shaderSimpleWireFrame" )
    //                   << QStringLiteral( "shadeWorldHeight" )
    //                   << QStringLiteral( "shadeWorldNormal")
    //                   << QStringLiteral( "shadeGrass" )
    //                   << QStringLiteral( "shadeGrassAndRocks" )
    //                   << QStringLiteral( "shadeGrassRocksAndSnow" )
    //                   << QStringLiteral( "shadeLightingFactors" )
    //                   << QStringLiteral( "shadeTexturedAndLit" );
}

void TerrainTessellationScene::onMessageLogged( QOpenGLDebugMessage message )
{
    qDebug() << message;
}

void TerrainTessellationScene::initialise()
{	    
	if(DEBUG_OPENGL_ENABLED)
	{
		connect( &m_logger, SIGNAL( messageLogged( QOpenGLDebugMessage ) ),
				 this, SLOT( onMessageLogged( QOpenGLDebugMessage ) ),
				 Qt::DirectConnection );
		if ( m_logger.initialize() ) {
			m_logger.startLogging( QOpenGLDebugLogger::SynchronousLogging );
			m_logger.enableMessages();
		}
	}
    m_funcs = m_context->versionFunctions<QOpenGLFunctions_4_2_Core>();
    if ( !m_funcs )
    {
        qFatal("Requires OpenGL >= 4.0");
        exit( 1 );
    }
    m_funcs->initializeOpenGLFunctions();
    // Initialize resources
    prepareShaders();
    //prepareTextures();
    prepareVertexBuffers( m_heightMapSize );

    prepareVertexArrayObject();

	//glCullFace


    // Set the wireframe line properties
    QOpenGLShaderProgramPtr shader = m_material->shader();
    shader->bind();
    shader->setUniformValue( "line.width", 1.0f );
    shader->setUniformValue( "line.color", QVector4D( 0.0f, 1.0f, 0.0f, 1.0f ) );

    // Set the fog parameters
    shader->setUniformValue( "fog.color", QVector4D( 0.65f, 0.77f, 1.0f, 1.0f ) );
    shader->setUniformValue( "fog.minDistance", 50.0f );
    shader->setUniformValue( "fog.maxDistance", 128.0f );
	
    shader->release();
    // Get subroutine indices
    //for ( int i = 0; i < DisplayModeCount; ++i)
    //{
    //    m_displayModeSubroutines[i] =
    //        m_funcs->glGetSubroutineIndex( shader->programId(),
    //                                       GL_FRAGMENT_SHADER,
    //                                       m_displayModeNames.at( i ).toLatin1() );
    //}
}

void TerrainTessellationScene::update( float t )
{
    m_modelMatrix.setToIdentity();
	m_modelMatrix.scale(10.f);

    // Store the time
    const float dt = t - m_time;
    m_time = t;

    // Update the camera position and orientation
    Camera::CameraTranslationOption option = m_viewCenterFixed
                                           ? Camera::DontTranslateViewCenter
                                           : Camera::TranslateViewCenter;
    m_camera->translate( m_v * dt * m_metersToUnits, option );

    if ( !qFuzzyIsNull( m_panAngle ) )
    {
        m_camera->pan( m_panAngle, QVector3D( 0.0f, 1.0f, 0.0f ) );
        m_panAngle = 0.0f;
    }

    if ( !qFuzzyIsNull( m_tiltAngle ) )
    {
        m_camera->tilt( m_tiltAngle );
        m_tiltAngle = 0.0f;
    }
}
void TerrainTessellationScene::setRootObject(QQuickItem* ctx)
{
	m_rootObject = ctx;
}

void TerrainTessellationScene::render()
{
    glClearColor( 0.65f, 0.77f, 1.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT );
	
    // Enable depth testing
	m_funcs->glDepthMask(GL_TRUE);
	m_funcs->glEnable( GL_DEPTH_TEST );
	m_funcs->glDepthFunc(GL_LESS);
    m_funcs->glEnable( GL_CULL_FACE );
	m_funcs->glCullFace(GL_BACK);
	m_funcs->glDisable(GL_BLEND);

    //m_material->bind();
    QOpenGLShaderProgramPtr shader = m_material->shader();
	
    shader->bind();
    //// Set the fragment shader display mode subroutine
    ////m_funcs->glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1,
    ////                                  &m_displayModeSubroutines[m_displayMode] );

    // Set the horizontal and vertical scales applied in the tess eval shader
    shader->setUniformValue( "horizontalScale", m_horizontalScale );
    shader->setUniformValue( "verticalScale", m_verticalScale );
    shader->setUniformValue( "pixelsPerTriangleEdge", m_screenSpaceError );

    // Pass in the usual transformation matrices
    QMatrix4x4 viewMatrix = m_camera->viewMatrix();
    QMatrix4x4 modelViewMatrix = viewMatrix * m_modelMatrix;
    QMatrix3x3 worldNormalMatrix = m_modelMatrix.normalMatrix();
    QMatrix3x3 normalMatrix = modelViewMatrix.normalMatrix();
    QMatrix4x4 mvp = m_camera->projectionMatrix() * modelViewMatrix;
    shader->setUniformValue( "WorldMatrix", m_modelMatrix );
    shader->setUniformValue( "ModelViewMatrix", modelViewMatrix );
    shader->setUniformValue( "WorldNormalMatrix", worldNormalMatrix );
    shader->setUniformValue( "NormalMatrix", normalMatrix );
    shader->setUniformValue( "ModelViewProjectionMatrix", mvp );

    // Set the lighting parameters
    QVector4D worldLightDirection( sinf( m_sunTheta * degToRad ), cosf( m_sunTheta * degToRad ), 0.0f, 0.0f );
    QMatrix4x4 worldToEyeNormal( normalMatrix );
    QVector4D lightDirection = worldToEyeNormal * worldLightDirection;

	//m_qmlContext->findChild<QObject>(QString("materialKa")).property("value").toDouble();
	double matKa = m_rootObject->property("material_Ka").toDouble();
	double matKd = m_rootObject->property("material_Kd").toDouble();
	double matKs = m_rootObject->property("material_Ks").toDouble();
	float matShininess = m_rootObject->property("material_shininess").toDouble();
    shader->setUniformValue( "light.position", lightDirection );
    shader->setUniformValue( "light.intensity", QVector3D( 1.0f, 1.0f, 1.0f ) );

    // Set the material properties
    shader->setUniformValue( "material.Ka", QVector3D( matKa, matKa, matKa ) );
    shader->setUniformValue( "material.Kd", QVector3D( matKd, matKd, matKd ) );
    shader->setUniformValue( "material.Ks", QVector3D( matKs, matKs, matKs ) );
    shader->setUniformValue( "material.shininess", matShininess );

    {
        QOpenGLVertexArrayObject::Binder binder( &m_vao );
        //shader->setPatchVertexCount( 1 );
		//m_funcs->glDrawElements(GL_TRIANGLES, 8, GL_UNSIGNED_INT, (const GLvoid*)0);
		// HACK
		m_funcs->glDrawElements(GL_TRIANGLES, m_elementCount, GL_UNSIGNED_INT, (GLvoid*)0);

		//m_funcs->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
	shader->release();
}

void TerrainTessellationScene::resize( int w, int h )
{
    // Make sure the viewport covers the entire window
    glViewport( 0, 0, w, h );

    m_viewportSize = QVector2D( float( w ), float( h ) );

    // Update the projection matrix
    float aspect = static_cast<float>( w ) / static_cast<float>( h );
    m_camera->setPerspectiveProjection( 55.0f, aspect, 0.2f, 2048.0f );

    // Update the viewport matrix
    float w2 = w / 2.0f;
    float h2 = h / 2.0f;
    m_viewportMatrix.setToIdentity();
    m_viewportMatrix.setColumn( 0, QVector4D( w2, 0.0f, 0.0f, 0.0f ) );
    m_viewportMatrix.setColumn( 1, QVector4D( 0.0f, h2, 0.0f, 0.0f ) );
    m_viewportMatrix.setColumn( 2, QVector4D( 0.0f, 0.0f, 1.0f, 0.0f ) );
    m_viewportMatrix.setColumn( 3, QVector4D( w2, h2, 0.0f, 1.0f ) );

    // We need the viewport size to calculate tessellation levels
    QOpenGLShaderProgramPtr shader = m_material->shader();
	shader->bind();
    shader->setUniformValue( "viewportSize", m_viewportSize );

    // The geometry shader also needs the viewport matrix
    shader->setUniformValue( "viewportMatrix", m_viewportMatrix );
	shader->release();
}

void TerrainTessellationScene::prepareShaders()
{
    m_material = MaterialPtr( new Material );
    //m_material->setShaders( ":/shaders/terraintessellation.vert",
    //                        //":/shaders/terraintessellation.tcs",
    //                        //":/shaders/terraintessellation.tes",
    //                        //":/shaders/terraintessellation.geom",
    //                        ":/shaders/terraintessellation.frag" );
	//m_material->setShadersFromString( VertexShader, FragmentShader);
	//m_material->setShaders( "resources/shaders/inline.vert", "resources/shaders/inline.frag");
	//m_material->setShaders( "resources/shaders/phong.vert", "resources/shaders/phong.frag");
	//m_material->setShaders( "resources/shaders/phong.vert", "resources/shaders/phongcomputenormalsflat.geom", "resources/shaders/phong.frag");
	m_material->setShaders( "resources/shaders/phong.vert", "resources/shaders/phong.frag");
}

/*void TerrainTessellationScene::prepareTextures()
{
    SamplerPtr sampler( new Sampler );
    sampler->create();
    sampler->setMinificationFilter( GL_LINEAR );
    sampler->setMagnificationFilter( GL_LINEAR );
    sampler->setWrapMode( Sampler::DirectionS, GL_CLAMP_TO_EDGE );
    sampler->setWrapMode( Sampler::DirectionT, GL_CLAMP_TO_EDGE );

    QImage heightMapImage( "../terrain_tessellation/heightmap-1024x1024.png" );
    m_funcs->glActiveTexture( GL_TEXTURE0 );
    TexturePtr heightMap( new Texture );
    heightMap->create();
    heightMap->bind();
    heightMap->setImage( heightMapImage );
    m_heightMapSize = heightMapImage.size();
    m_material->setTextureUnitConfiguration( 0, heightMap, sampler, QByteArrayLiteral( "heightMap" ) );

    SamplerPtr tilingSampler( new Sampler );
    tilingSampler->create();
    tilingSampler->setMinificationFilter( GL_LINEAR_MIPMAP_LINEAR );
    m_funcs->glSamplerParameterf( tilingSampler->samplerId(), GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f );
    tilingSampler->setMagnificationFilter( GL_LINEAR );
    tilingSampler->setWrapMode( Sampler::DirectionS, GL_REPEAT );
    tilingSampler->setWrapMode( Sampler::DirectionT, GL_REPEAT );

    QImage grassImage( "../terrain_tessellation/grass.png" );
    m_funcs->glActiveTexture( GL_TEXTURE1 );
    TexturePtr grassTexture( new Texture );
    grassTexture->create();
    grassTexture->bind();
    grassTexture->setImage( grassImage );
    grassTexture->generateMipMaps();
    m_material->setTextureUnitConfiguration( 1, grassTexture, tilingSampler, QByteArrayLiteral( "grassTexture" ) );

    QImage rockImage( "../terrain_tessellation/rock.png" );
    m_funcs->glActiveTexture( GL_TEXTURE2 );
    TexturePtr rockTexture( new Texture );
    rockTexture->create();
    rockTexture->bind();
    rockTexture->setImage( rockImage );
    rockTexture->generateMipMaps();
    m_material->setTextureUnitConfiguration( 2, rockTexture, tilingSampler, QByteArrayLiteral( "rockTexture" ) );

    QImage snowImage( "../terrain_tessellation/snowrocks.png" );
    m_funcs->glActiveTexture( GL_TEXTURE3 );
    TexturePtr snowTexture( new Texture );
    snowTexture->create();
    snowTexture->bind();
    snowTexture->setImage( snowImage );
    snowTexture->generateMipMaps();
    m_material->setTextureUnitConfiguration( 3, snowTexture, tilingSampler, QByteArrayLiteral( "snowTexture" ) );

    m_funcs->glActiveTexture( GL_TEXTURE0 );
}*/

void TerrainTessellationScene::prepareVertexBuffers( QSize heightMapSize )
{
	//QFile file("shaders/truhe.obj");
	QFile file("resources/objects/bunny.obj3d");
	//QFile file("resources/objects/san-miguel.obj");
	qCritical(QDir::currentPath().toLatin1().data());
	LoaderObj loader = LoaderObj(file);
	m_elementCount = loader.getIndexCount();
	m_vertexCount = loader.getVertexCount();
	
	//Buffer filled by compute shader
    m_normalsBuffer.create();
    m_normalsBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_normalsBuffer.bind();
	GLfloat *pB = loader.getCalculatedNormals();
	m_normalsBuffer.allocate(pB, loader.getVertexCount()*3*sizeof(GLfloat) );
    m_normalsBuffer.release();

    m_vertexBuffer.create();
    m_vertexBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_vertexBuffer.bind();
	GLfloat *pVB = loader.getVB();
	m_vertexBuffer.allocate( pVB, m_vertexCount*3*sizeof(GLfloat) );
    m_vertexBuffer.release();
	
    m_indexBuffer.create();
    m_indexBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_indexBuffer.bind();
	GLuint *pIB = loader.getIB();
	m_indexBuffer.allocate( pIB, loader.getIndexCount()*sizeof(GLuint) );
    m_indexBuffer.release();

	//genNormalsGPU();

	int stride = 3*sizeof(float);
    m_vao.create();
    {
        QOpenGLVertexArrayObject::Binder binder( &m_vao );
        QOpenGLShaderProgramPtr shader = m_material->shader();
        shader->bind();
        m_vertexBuffer.bind();
        //shader->setAttributeBuffer( "vertexPosition", GL_FLOAT, 0, 3 );
		shader->setAttributeBuffer("in_Position", GL_FLOAT, 0, 3, stride);
		//shader->setAttributeBuffer("in_Color", GL_FLOAT, 0, 3, stride);
        shader->enableAttributeArray( "in_Position" );
        //shader->enableAttributeArray( "in_Color" );

        m_normalsBuffer.bind();
		shader->setAttributeBuffer("in_Normal", GL_FLOAT, 0, 3, stride);
        shader->enableAttributeArray( "in_Normal" );
		//glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, VertexSize, 0);
		//glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)RgbOffset);
		m_indexBuffer.bind();
    }
    m_material->shader()->release();
    m_vertexBuffer.release();
    m_normalsBuffer.release();
    m_indexBuffer.release();
}

//void TerrainTessellationScene::prepareVertexBuffers( QSize heightMapSize )
//{
//
//    m_vertexBuffer.create();
//    m_vertexBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
//    m_vertexBuffer.bind();
//    //m_vertexBuffer.allocate( octa_verts, 6 * 3 * sizeof( float ) );
//    m_vertexBuffer.allocate( Vertices, sizeof(Vertices) );
//    m_vertexBuffer.release();
//	
//    m_indexBuffer.create();
//    m_indexBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
//    m_indexBuffer.bind();
//    //m_indexBuffer.allocate( octa_indices, 8 * 3 * sizeof( int ) );
//	m_indexBuffer.allocate( Indices, sizeof(Indices) );
//    m_indexBuffer.release();
//}

void TerrainTessellationScene::prepareVertexArrayObject()
{
	//GLenum ErrorCheckValue = glGetError();
	//const size_t BufferSize = sizeof(Vertices);
	//const size_t VertexSize = sizeof(Vertices[0]);
	//const size_t RgbOffset = sizeof(Vertices[0].XYZW);
 //   // Create a VAO for this "object"
 //   m_vao.create();
 //   {
 //       QOpenGLVertexArrayObject::Binder binder( &m_vao );
 //       QOpenGLShaderProgramPtr shader = m_material->shader();
 //       shader->bind();
 //       m_vertexBuffer.bind();
 //       //shader->setAttributeBuffer( "vertexPosition", GL_FLOAT, 0, 3 );
	//	shader->setAttributeBuffer("in_Position", GL_FLOAT, 0, 3, 6*sizeof(float));
	//	shader->setAttributeBuffer("in_Color", GL_FLOAT, RgbOffset, 3, 6*sizeof(float));
 //       shader->enableAttributeArray( "in_Position" );
 //       shader->enableAttributeArray( "in_Color" );
	//	//glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, VertexSize, 0);
	//	//glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)RgbOffset);
	//	m_indexBuffer.bind();
 //   }
}

/*
void TerrainTessellationScene::genNormalsGPU() {
	// Creating the compute shader, and the program object containing the shader
    GLuint progHandle = m_funcs->glCreateProgram();
    GLuint cs = m_funcs->glCreateShader(GL_COMPUTE_SHADER);
	
    QFile file("shaders/genNormals.comp");
    file.open(QIODevice::ReadOnly);
    QString s;

    s.append(file.readAll());
	const GLchar *const ptr = s.toLatin1().data();
    m_funcs->glShaderSource(cs, 2, &ptr, NULL);
	m_funcs->glCompileShader(cs);
    int rvalue;
    m_funcs->glGetShaderiv(cs, GL_COMPILE_STATUS, &rvalue);
    if (!rvalue) {
        fprintf(stderr, "Error in compiling the compute shader\n");
        GLchar log[10240];
        GLsizei length;
        m_funcs->glGetShaderInfoLog(cs, 10239, &length, log);
        fprintf(stderr, "Compiler log:\n%s\n", log);
        exit(40);
    }
    m_funcs->glAttachShader(progHandle, cs);

    m_funcs->glLinkProgram(progHandle);
    m_funcs->glGetProgramiv(progHandle, GL_LINK_STATUS, &rvalue);
    if (!rvalue) {
        fprintf(stderr, "Error in linking compute shader program\n");
        GLchar log[10240];
        GLsizei length;
        m_funcs->glGetProgramInfoLog(progHandle, 10239, &length, log);
        fprintf(stderr, "Linker log:\n%s\n", log);
        exit(41);
    }   
	m_funcs->glUseProgram(progHandle);
    
	//m_funcs->glUniform1i(m_funcs->glGetUniformLocation(progHandle, "normalsBuff"), m_normalsBuffer.bufferId);

	GLuint normalsBufferId = m_funcs->glGetProgramResourceIndex(progHandle, GL_SHADER_STORAGE_BLOCK, "normalsBuffer");
	m_funcs->glShaderStorageBlockBinding(progHandle, m_normalsBuffer.bufferId(), normalsBufferId);

	m_funcs->glUseProgram(progHandle);
	m_funcs->glUniform1f(m_funcs->glGetUniformLocation(progHandle, "maxFaces"), 0);
	m_funcs->glDispatchCompute(m_vertexCount, m_elementCount, 1);
	m_funcs->glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}*/