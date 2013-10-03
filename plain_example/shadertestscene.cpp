﻿#include "shadertestscene.h"
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
#include <qscopedpointer.h>

#include "LoaderObj.h"

#define _USE_MATH_DEFINES
#include <math.h>

const float degToRad = float( M_PI / 180.0 );

ShaderTestScene::ShaderTestScene( QObject* parent )
    : AbstractScene( parent ),
      m_v(),
      m_viewCenterFixed( false ),
      m_panAngle( 0.0f ),
      m_tiltAngle( 0.0f ),
      m_normalsBuffer( QOpenGLBuffer::VertexBuffer ),
      m_vertexBuffer( QOpenGLBuffer::VertexBuffer ),
      m_indexBuffer( QOpenGLBuffer::IndexBuffer ),
      m_screenSpaceError( 12.0f ),
      m_modelMatrix(),
      m_time( 0.0f ),
      m_metersToUnits( 0.05f ), // 500 units == 10 km => 0.05 units/m
      m_funcs( 0 ),
	  m_material( 0 ),
	  m_isInitialized(false),
	  m_cameraMode(CameraMode::CAMERMODE_WALKTHROUGH),
	  m_glCullMode(GL_BACK),
	  m_rotationSpeed(0.0f),
	  m_position( -1.3f, 1.5f, 2.0f ),
	  m_viewCenter( 0.0f, 1.1f, 0.0f ),
	  m_upVector(0.0f, 1.0f, 0.0f ),
	  m_cameraToCenter(m_viewCenter-m_position)
{
}

void ShaderTestScene::onMessageLogged( QOpenGLDebugMessage message )
{
    qDebug() << message;
}

void ShaderTestScene::initialise()
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
        qFatal("Requires OpenGL >= 4.0 and Qt compatible Profile.");
        exit( 1 );
    }
    m_funcs->initializeOpenGLFunctions();
    // Initialize resources
    recompileShader();
    prepareVertexBuffers();

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
	
    m_modelMatrix.setToIdentity();
	//m_modelMatrix.scale(0.05f); // Crytek sponza
	m_modelMatrix.scale(10.f); // bunny

	m_isInitialized = true;
	//Set ShaderUniforms that the Ui wanted to set during initialisation
	QHashIterator<QString, float> iter1f(m_initialUniforms1f);
	while (iter1f.hasNext()) {
		iter1f.next();
		setShaderUniformValue(iter1f.key().toStdString().c_str(), iter1f.value());
	}
	QHashIterator<QString, int> iter1i(m_initialUniforms1i);
	while (iter1i.hasNext()) {
		iter1i.next();
		setShaderUniformValue(iter1i.key().toStdString().c_str(), iter1i.value());
	}
	QHashIterator<QString, QVector3D> iter3f(m_initialUniforms3f);
	while (iter3f.hasNext()) {
		iter3f.next();
		setShaderUniformValue(iter3f.key().toStdString().c_str(), iter3f.value().x(), iter3f.value().y(), iter3f.value().z());
	}
}

void ShaderTestScene::update( float t )
{

    // Store the time
    const float dt = t - m_time;
    m_time = t;
	
	m_modelMatrix.rotate(m_rotationSpeed*dt, 0.f, 1.f, 0.0f);

	if(m_cameraMode == CAMERMODE_WALKTHROUGH)
	{
		// Update the camera position and orientation
		translate( m_v * dt * m_metersToUnits );

		if ( !qFuzzyIsNull( m_panAngle ) )
		{
			pan( m_panAngle );
			m_panAngle = 0.0f;
		}

		if ( !qFuzzyIsNull( m_tiltAngle ) )
		{
			tilt( m_tiltAngle );
			m_tiltAngle = 0.0f;
		}
	} else {

		if ( !qFuzzyIsNull( m_panAngle ) )
		{
			pan( m_panAngle );
			m_panAngle = 0.0f;
		}

		if ( !qFuzzyIsNull( m_tiltAngle ) )
		{
			tilt( m_tiltAngle );
			m_tiltAngle = 0.0f;
		}
	}
}

void ShaderTestScene::render()
{
    m_funcs->glClearColor( 0.65f, 0.77f, 1.0f, 1.0f );
    m_funcs->glClear( GL_COLOR_BUFFER_BIT );
	
    // Enable depth testing
	m_funcs->glDepthMask(GL_TRUE);
	m_funcs->glEnable( GL_DEPTH_TEST );
	m_funcs->glDepthFunc(GL_LESS);
    m_funcs->glEnable( GL_CULL_FACE );
	m_funcs->glCullFace(m_glCullMode); //TODO: Ausprobieren, Einstellbar machen
	m_funcs->glEnable(GL_BLEND);

	//Avoid flickering after Qt Ui updates. Qt updates only a portion of the screen.
	//If we do not reset viewport size, our rendering will be of the size of the last gui-element qt drawed.
	m_funcs->glViewport( 0, 0, m_viewportSize.x(), m_viewportSize.y() );
    //m_material->bind();
    QOpenGLShaderProgramPtr shader = m_material->shader();
	
    shader->bind();
    //// Set the fragment shader display mode subroutine
    ////m_funcs->glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1,
    ////                                  &m_displayModeSubroutines[m_displayMode] );

    // Pass in the usual transformation matrices
    m_viewMatrix.setToIdentity();
    m_viewMatrix.lookAt( m_position, m_viewCenter, m_upVector );
    QMatrix4x4 modelViewMatrix = m_viewMatrix * m_modelMatrix;
    QMatrix3x3 worldNormalMatrix = m_modelMatrix.normalMatrix();
    QMatrix3x3 normalMatrix = modelViewMatrix.normalMatrix();
    QMatrix4x4 mvp = m_projectionMatrix * modelViewMatrix;
    shader->setUniformValue( "ModelMatrix", m_modelMatrix );
    shader->setUniformValue( "ModelViewMatrix", modelViewMatrix );
    shader->setUniformValue( "ModelNormalMatrix", worldNormalMatrix );
    shader->setUniformValue( "NormalMatrix", normalMatrix );
    shader->setUniformValue( "ModelViewProjectionMatrix", mvp );

	//m_qmlContext->findChild<QObject>(QString("materialKa")).property("value").toDouble();
	//double matKa = m_rootObject->property("material_Ka").toDouble();
	//double matKd = m_rootObject->property("material_Kd").toDouble();
	//double matKs = m_rootObject->property("material_Ks").toDouble();
	//float matShininess = m_rootObject->property("material_shininess").toDouble();
    //shader->setUniformValue( "light.position", lightDirection );
    shader->setUniformValue( "light.intensity", QVector3D( 1.0f, 1.0f, 1.0f ) );

    // Set the material properties
    //shader->setUniformValue( "material.Ka", QVector3D( matKa, matKa, matKa ) );
    //shader->setUniformValue( "material.Kd", QVector3D( matKd, matKd, matKd ) );
    //shader->setUniformValue( "material.Ks", QVector3D( matKs, matKs, matKs ) );
    //shader->setUniformValue( "material.shininess", matShininess );

	// Binder class calls m_vao.bind() in it's constructor and m_vao.release() in the destructor.
	// This let's us express bind/release calls with braces, as they define the scope of the Binder object.
	// Not all classes have binder defined.
	m_material->bind();
    {
        QOpenGLVertexArrayObject::Binder binder( &m_vao );
		m_funcs->glDrawElements(GL_TRIANGLES, m_elementCount, GL_UNSIGNED_INT, (GLvoid*)0);
    }
	shader->release();
	//m_material->release();
	m_funcs->glActiveTexture( GL_TEXTURE0 );
}

void ShaderTestScene::resize( int w, int h )
{
    // Make sure the viewport covers the entire window
	m_funcs->glViewport( 0, 0, w, h );

    m_viewportSize = QVector2D( float( w ), float( h ) );

    // Update the projection matrix
	float fieldOfView = 55.0f;
	float aspectRatio = static_cast<float>( w ) / static_cast<float>( h );
	float nearPlane = 0.2f;
	float farPlane = 1024.0f;
	m_projectionMatrix.setToIdentity();
    m_projectionMatrix.perspective( fieldOfView, aspectRatio, nearPlane, farPlane );

	//TODO: remove in simplke example?!
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

void ShaderTestScene::translate( const QVector3D& vLocal)
{
	if(vLocal.x() > 0.0f)
		qDebug() << "TATATATAT" << vLocal;
	QVector3D vWorld;
	QVector3D x = QVector3D::crossProduct( m_cameraToCenter, m_upVector ).normalized();
    vWorld += vLocal.x() * x;
    vWorld += vLocal.y() * m_upVector;
    vWorld += vLocal.z() * m_cameraToCenter.normalized();
    // Update the camera position using the calculated world vector
    m_position += vWorld;
    // Also update the view center coordinates
    m_viewCenter += vWorld;
    //m_cameraToCenter = m_viewCenter - m_position;
    // Calculate a new up vector. We do this by:
    // 1) Calculate a new local x-direction vector from the cross product of the new
    //    camera to view center vector and the old up vector.
    // 2) The local x vector is the normal to the plane in which the new up vector
    //    must lay. So we can take the cross product of this normal and the new
    //    x vector. The new normal vector forms the last part of the orthonormal basis
    //x = QVector3D::crossProduct( m_cameraToCenter, m_upVector ).normalized();
    //m_upVector = QVector3D::crossProduct( x, m_cameraToCenter ).normalized();
}

void ShaderTestScene::pan( const float &angle )
{
	QQuaternion q = QQuaternion::fromAxisAndAngle( QVector3D(0.0f,1.0f,0.0f), -angle );
    m_upVector = q.rotatedVector( m_upVector );
    m_cameraToCenter = q.rotatedVector( m_cameraToCenter );
    m_viewCenter = m_position + m_cameraToCenter;
}

void ShaderTestScene::tilt( const float &angle )
{
	QVector3D xBasis = QVector3D::crossProduct( m_upVector, m_cameraToCenter.normalized() ).normalized();
    QQuaternion q = QQuaternion::fromAxisAndAngle( xBasis, angle );
    m_upVector = q.rotatedVector( m_upVector );
    m_cameraToCenter = q.rotatedVector( m_cameraToCenter );
    m_viewCenter = m_position + m_cameraToCenter;
}

void ShaderTestScene::recompileShader()
{
	prepareShaders();
	prepareTextures();
}

void ShaderTestScene::prepareShaders()
{
	//if(m_material.isNull())
	if(m_material == 0)
	{
		//m_material = MaterialPtr( new Material );
		m_material = new Material;
	} else {
		m_material->shader()->release();
		//delete m_material;
		m_material = new Material;
		//m_material.reset(new Material);
	}
    //m_material->setShaders( ":/shaders/terraintessellation.vert",
    //                        //":/shaders/terraintessellation.tcs",
    //                        //":/shaders/terraintessellation.tes",
    //                        //":/shaders/terraintessellation.geom",
    //                        ":/shaders/terraintessellation.frag" );
	//m_material->setShadersFromString( VertexShader, FragmentShader);
	//m_material->setShaders( "resources/shaders/inline.vert", "resources/shaders/inline.frag");
	//m_material->setShaders( "resources/shaders/phong.vert", "resources/shaders/phong.frag");
	//m_material->setShaders( "resources/shaders/phong.vert", "resources/shaders/phongcomputenormalsflat.geom", "resources/shaders/phong.frag");
	m_material->setShaders( m_shaderInfo.vertexShaderFile, m_shaderInfo.fragmentShaderFile);
}

void ShaderTestScene::prepareTextures()
{
	SamplerPtr sampler( new Sampler );
    sampler->create();
    sampler->setMinificationFilter( GL_LINEAR );
    sampler->setMagnificationFilter( GL_LINEAR );
    sampler->setWrapMode( Sampler::DirectionS, GL_CLAMP_TO_EDGE );
    sampler->setWrapMode( Sampler::DirectionT, GL_CLAMP_TO_EDGE );

    QImage heightMapImage( "./resources/textures/grass.png" );
    m_funcs->glActiveTexture( GL_TEXTURE0 );
    TexturePtr heightMap( new Texture );
    heightMap->create();
    heightMap->bind();
    heightMap->setImage( heightMapImage );
    m_material->setTextureUnitConfiguration( 0, heightMap, sampler, QByteArrayLiteral( "heightMap" ) );

    SamplerPtr tilingSampler( new Sampler );
    tilingSampler->create();
    tilingSampler->setMinificationFilter( GL_LINEAR_MIPMAP_LINEAR );
    m_funcs->glSamplerParameterf( tilingSampler->samplerId(), GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f );
    tilingSampler->setMagnificationFilter( GL_LINEAR );
    tilingSampler->setWrapMode( Sampler::DirectionS, GL_REPEAT );
    tilingSampler->setWrapMode( Sampler::DirectionT, GL_REPEAT );

    QImage grassImage( "./resources/textures/grass.png" );
    m_funcs->glActiveTexture( GL_TEXTURE1 );
    TexturePtr grassTexture( new Texture );
    grassTexture->create();
    grassTexture->bind();
    grassTexture->setImage( grassImage );
    grassTexture->generateMipMaps();
    m_material->setTextureUnitConfiguration( 1, grassTexture, tilingSampler, QByteArrayLiteral( "grassTexture" ) );

    QImage rockImage( "./resources/textures/rock.png" );
    m_funcs->glActiveTexture( GL_TEXTURE2 );
    TexturePtr rockTexture( new Texture );
    rockTexture->create();
    rockTexture->bind();
    rockTexture->setImage( rockImage );
    rockTexture->generateMipMaps();
    m_material->setTextureUnitConfiguration( 2, rockTexture, tilingSampler, QByteArrayLiteral( "rockTexture" ) );

    QImage snowImage( "./resources/textures/snowrocks.png" );
    m_funcs->glActiveTexture( GL_TEXTURE3 );
    TexturePtr snowTexture( new Texture );
    snowTexture->create();
    snowTexture->bind();
    snowTexture->setImage( snowImage );
    snowTexture->generateMipMaps();
    m_material->setTextureUnitConfiguration( 3, snowTexture, tilingSampler, QByteArrayLiteral( "snowTexture" ) );

    m_funcs->glActiveTexture( GL_TEXTURE0 );
}

void ShaderTestScene::prepareVertexBuffers()
{
	//QFile file("shaders/truhe.obj");
	//QFile file("resources/objects/bunny.obj3d");
	//QFile file("resources/objects/san-miguel.obj");
	LoaderObj loader = LoaderObj("resources/objects/bunny.obj3d");
	//LoaderObj loader = LoaderObj("resources/objects/san-miguel.obj");
	//LoaderObj loader = LoaderObj("resources/objects/crytek-sponza/sponza.obj");
	m_elementCount = loader.getIndexCount();
	m_vertexCount = loader.getVertexCount();
	
	//Buffer filled by compute shader
    m_normalsBuffer.create();
    m_normalsBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_normalsBuffer.bind();
	GLfloat *pB = loader.getCalculatedNormals();
	m_normalsBuffer.allocate(pB, loader.getVertexCount()*3*sizeof(GLfloat) ); //TODO: Kommentar (GPU)
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
	//TODO: Kommentar, nach vao alles releasen, nicht automatisch
    m_material->shader()->release();
    m_vertexBuffer.release();
    m_normalsBuffer.release();
    m_indexBuffer.release();
}

void ShaderTestScene::setShaderUniformValue(const char *name, const float &val)
{
	if(!m_isInitialized)
	{
		m_initialUniforms1f[QString(name)] = val;
		return;
	}
	qDebug() << "Set Uniform \"" << name << "\": " << val;
	QOpenGLShaderProgramPtr shader = m_material->shader();
	shader->bind();

    // Set the lighting parameters
	if(QString(name) == QString("lightTheta"))
	{
		QVector4D worldLightDirection( sinf( val * degToRad )*50.f, cosf( val * degToRad )*50.f, 0.0f, 0.0f );
		//QMatrix4x4 worldToEyeNormal( (m_camera->viewMatrix() * m_modelMatrix).normalMatrix() );
		QVector4D lightDirection = (m_viewMatrix * m_modelMatrix) * worldLightDirection;
		shader->setUniformValue( "light.position", lightDirection );
	} else {
		shader->setUniformValue( name, val );
	}
	shader->release();
}

void ShaderTestScene::setShaderUniformValue(const char *name, const GLint &val)
{
	if(!m_isInitialized)
	{
		m_initialUniforms1i[QString(name)] = val;
		return;
	}
	qDebug() << "Set Uniform \"" << name << "\": " << val;
	QOpenGLShaderProgramPtr shader = m_material->shader();
	shader->bind();
	shader->setUniformValue( name, val );
	shader->release();
}

void ShaderTestScene::setShaderUniformValue(const char *name, const float &x, const float &y, const float &z)
{
	if(!m_isInitialized)
	{
		m_initialUniforms3f[QString(name)] = QVector3D(x,y,z);
		return;
	}
	qDebug() << "Set Uniform \"" << name << "\": (" << x << ", " << y << ", " << z << ")";
	QOpenGLShaderProgramPtr shader = m_material->shader();
	shader->bind();
	shader->setUniformValue( name, QVector3D( x, y, z ) );
	shader->release();
}

void ShaderTestScene::setActiveShader(const ShaderInfo &shader)
{
	m_shaderInfo.fragmentShaderFile = shader.fragmentShaderFile;
	m_shaderInfo.fragmentShaderProc = shader.fragmentShaderProc;
	m_shaderInfo.vertexShaderFile = shader.vertexShaderFile;
	m_shaderInfo.vertexShaderProc = shader.vertexShaderProc;
	if(m_isInitialized)
	{
		recompileShader();
	}
}
/*
void ShaderTestScene::genNormalsGPU() {
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