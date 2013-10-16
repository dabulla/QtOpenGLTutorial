#include "shadertestscene.h"
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
#include <qvector.h>

#include "LoaderObj.h"

#define _USE_MATH_DEFINES
#include <math.h>

const float degToRad = float( M_PI / 180.0 );

ShaderTestScene::ShaderTestScene( QObject* parent )
	: QObject( parent ),
      m_panAngle( 0.0f ),
      m_tiltAngle( 0.0f ),
      m_positionBuffer( QOpenGLBuffer::VertexBuffer ),
      m_normalsBuffer( QOpenGLBuffer::VertexBuffer ),
      m_tangentsBuffer( QOpenGLBuffer::VertexBuffer ),
      m_bitangentsBuffer( QOpenGLBuffer::VertexBuffer ),
	  m_texCoordsBuffer( QOpenGLBuffer::VertexBuffer ),
      m_indexBuffer( QOpenGLBuffer::IndexBuffer ),
      m_planePositionBuffer( QOpenGLBuffer::VertexBuffer ),
      m_planeNormalsBuffer( QOpenGLBuffer::VertexBuffer ),
	  m_planeTangentsBuffer( QOpenGLBuffer::VertexBuffer ),
	  m_planeBitangentsBuffer( QOpenGLBuffer::VertexBuffer ),
	  m_planeTexCoordsBuffer( QOpenGLBuffer::VertexBuffer ),
      m_planeIndexBuffer( QOpenGLBuffer::IndexBuffer ),
      m_screenSpaceError( 12.0f ),
      m_modelMatrix(),
      m_time( 0.0f ),
      m_metersToUnits( 0.05f ), // 500 units == 10 km => 0.05 units/m
      m_funcs( 0 ),
	  m_material( 0 ),
	  m_isInitialized(false),
	  m_cameraMode(CameraMode::CAMERMODE_OBJECTINSPECTION),
	  m_currentObject(CurrentObject::OBJECT_BUNNY),
	  m_glCullMode(GL_BACK),
	  m_rotationSpeed(0.0f),
	  m_position( -1.3f, 1.5f, 2.0f ),
	  m_viewCenter( 0.0f, 1.1f, 0.0f ),
	  m_upVector(0.0f, 1.0f, 0.0f ),
	  m_cameraToCenter(m_viewCenter-m_position),
	  m_tilingSamplerId(0),
	  m_planeResolutionX(8),
	  m_planeResolutionZ(8),
	  m_planeVertexCount(m_planeResolutionX*m_planeResolutionZ),
	  m_planeElementCount((m_planeResolutionX-1)*(m_planeResolutionZ-1)*2*3) // For each Vertex there will be two triangles, except the last row and column of the plane.
{
}

ShaderTestScene::~ShaderTestScene()
{
	//if(m_tilingSamplerId) {
 //       m_funcs->glDeleteSamplers( 1, &m_tilingSamplerId );
	//}
}

void ShaderTestScene::onMessageLogged( QOpenGLDebugMessage message )
{
    qDebug() << message;
}

void ShaderTestScene::initialise()
{
#ifdef DEBUG_OPENGL_ENABLED
	connect( &m_logger, SIGNAL( messageLogged( QOpenGLDebugMessage ) ),
				this, SLOT( onMessageLogged( QOpenGLDebugMessage ) ),
				Qt::DirectConnection );
	if ( m_logger.initialize() ) {
		m_logger.startLogging( QOpenGLDebugLogger::SynchronousLogging );
		m_logger.enableMessages();
		QVector<uint> disabledMessages;
		disabledMessages.push_back(131185);
		disabledMessages.push_back(131204);
		disabledMessages.push_back(131218);
		disabledMessages.push_back(131184);
		m_logger.disableMessages(disabledMessages);
	}
#endif
    m_funcs = m_context->versionFunctions<QOpenGLFunctions_4_2_Core>();
    if ( !m_funcs )
    {
        qFatal("Requires OpenGL >= 4.0 and Qt compatible Profile.");
        exit( 1 );
    }
    m_funcs->initializeOpenGLFunctions();
	m_isInitialized = true;
    // Initialize resources
    recompileShader();
    prepareVertexBuffers();

	// some constant uniforms are set once after initialisation
	// for other uniforms, see passUniforms()

    // Set the wireframe line properties
    QOpenGLShaderProgramPtr shader = m_material->shader();
    shader->bind();
	setShaderUniformValue("line.width", 1.0f);
	setShaderUniformValue("line.color", 0.0f, 0.6f, 0.0f);

    // Set the fog parameters
    shader->setUniformValue( "fog.color", QVector4D( 0.65f, 0.77f, 1.0f, 1.0f ) );
    shader->setUniformValue( "fog.minDistance", 50.0f );
    shader->setUniformValue( "fog.maxDistance", 128.0f );
	
    shader->release();
	
    m_modelMatrix.setToIdentity();
	//m_modelMatrix.scale(0.05f); // Crytek sponza
	m_modelMatrix.scale(10.f); // bunny
}

void ShaderTestScene::update( float t )
{

    // Store the time
    const float dt = t - m_time;
    m_time = t;

	m_offset += m_velocity;

	m_modelMatrix.rotate(m_rotationSpeed*dt, 0.f, 1.f, 0.0f);

	if(m_cameraMode == CAMERMODE_WALKTHROUGH)
	{
		// Update the camera position and orientation
		translate( m_offset * dt * m_metersToUnits );

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
		translate( m_offset * dt * m_metersToUnits );
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
	m_offset.setX(0);
	m_offset.setY(0);
	m_offset.setZ(0);
}

void ShaderTestScene::render()
{
    m_funcs->glClearColor( 0.65f, 0.77f, 1.0f, 1.0f );
	m_funcs->glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
    // Enable depth testing
	m_funcs->glDepthMask(GL_TRUE);
	m_funcs->glEnable( GL_DEPTH_TEST );
	m_funcs->glDepthFunc(GL_LESS);
	if(m_glCullMode == GL_NONE)
	{
		m_funcs->glDisable( GL_CULL_FACE );
	} else {
		m_funcs->glEnable( GL_CULL_FACE );
		m_funcs->glCullFace(m_glCullMode);
	}
	m_funcs->glEnable(GL_BLEND);
	//m_funcs->glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Avoid flickering after Qt Ui updates. Qt updates only a portion of the screen.
	//If we do not reset viewport size, our rendering will be of the size of the last gui-element qt drawed.
	m_funcs->glViewport( 0, 0, m_viewportSize.x(), m_viewportSize.y() );

	//Bind and activate all Textures and Sampler
    m_material->bind();

	//Before setting uniforms, the shader must be bound.
	//Note: m_material->bind(); already called shader->bind() do not forget to call shader->release()
    //shader->bind();
	passUniforms();

	int renderPasses = m_initialUniforms1i.value("renderPasses", 1);

	int elementCount;
	QOpenGLVertexArrayObject* vao;
	switch(m_currentObject)
	{
	case CurrentObject::OBJECT_BUNNY:
		elementCount = m_elementCount;
		vao = &m_vaoBunny;
		break;
	case CurrentObject::OBJECT_PLANE:
		elementCount = m_planeElementCount;
		vao = &m_vaoPlane;
		break;
	}
	// Binder class calls m_vao.bind() in it's constructor and m_vao.release() in the destructor.
	// This let's us express bind/release calls with braces, as they define the scope of the Binder object.
	// Not all classes have binder defined.
    {
        QOpenGLVertexArrayObject::Binder binder( vao );
		//Render multiple passes (e.g. for fur). This can not alwasy be done in a geometry shader.
		// If transparency is involved, one would want to draw far primitives first.
		for(int renderPass=0 ; renderPass < renderPasses ; renderPass++)
		{
			m_material->shader()->setUniformValue("renderPass", renderPass);
			if(m_shaderInfo.tesselationControlShaderFile.isEmpty())
			{
				m_funcs->glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_INT, (GLvoid*)0);
			} else {
				m_material->shader()->setPatchVertexCount( 3 );
				m_funcs->glDrawElements(GL_PATCHES, elementCount, GL_UNSIGNED_INT, (GLvoid*)0);
			}
		}
    }
	m_material->shader()->release();
	//m_material->release();
	m_funcs->glActiveTexture( GL_TEXTURE0 );
}

//Precondition: Shader must be bound (shader->bind())
void ShaderTestScene::passUniforms()
{
    QOpenGLShaderProgramPtr shader = m_material->shader();

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
	
	// Set the active Shader subroutine
	// Subroutines are functions in the shader. They can be set almost in the same way as other uniform values
	// In contrast to other uniform types (e.g. "float"), the possible values of a subroutine variable are defined in the shader itself (e.g. "applyPhongLighting()", "applyFlatLighting()")
	// Because of that we have to look-up the given name of a function and then set the function to a variable in the shader.
	// Using this technique, multiple shader can be implemented in one file. Functions can be reused (eg. phong+texturing).
		
	// 1) Look up the subroutine-location for the name from "m_shaderInfo"
	// 2) Set the first subroutine-variable in a shader to the found subroutine.
	if(!m_shaderInfo.vertexShaderProc.isEmpty())
	{
		GLuint vertexShaderSubroutineLocation = m_funcs->glGetSubroutineIndex(shader->programId(), GL_VERTEX_SHADER,
			m_shaderInfo.vertexShaderProc.toStdString().c_str());
		m_funcs->glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1,
			&vertexShaderSubroutineLocation );
	}
	if(!m_shaderInfo.tesselationControlShaderProc.isEmpty())
	{
		GLuint tesselationControlShaderSubroutineLocation = m_funcs->glGetSubroutineIndex(shader->programId(), GL_TESS_CONTROL_SHADER,
			m_shaderInfo.tesselationControlShaderProc.toStdString().c_str());
		m_funcs->glUniformSubroutinesuiv( GL_TESS_CONTROL_SHADER, 1,
			&tesselationControlShaderSubroutineLocation );
	}
	if(!m_shaderInfo.tesselationEvaluationShaderProc.isEmpty())
	{
		GLuint tesselationEvaluationShaderSubroutineLocation = m_funcs->glGetSubroutineIndex(shader->programId(), GL_TESS_EVALUATION_SHADER,
			m_shaderInfo.tesselationEvaluationShaderProc.toStdString().c_str());
		m_funcs->glUniformSubroutinesuiv( GL_TESS_EVALUATION_SHADER, 1,
			&tesselationEvaluationShaderSubroutineLocation );
	}
	if(!m_shaderInfo.geometryShaderProc.isEmpty())
	{
		GLuint geometryShaderSubroutineLocation = m_funcs->glGetSubroutineIndex(shader->programId(), GL_GEOMETRY_SHADER,
			m_shaderInfo.geometryShaderProc.toStdString().c_str());
		m_funcs->glUniformSubroutinesuiv( GL_GEOMETRY_SHADER, 1,
			&geometryShaderSubroutineLocation );
	}
	if(!m_shaderInfo.fragmentShaderProc.isEmpty())
	{
		GLuint fragmentShaderSubroutineLocation = m_funcs->glGetSubroutineIndex(shader->programId(), GL_FRAGMENT_SHADER,
			m_shaderInfo.fragmentShaderProc.toStdString().c_str());
		m_funcs->glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1,
			&fragmentShaderSubroutineLocation );
	}

	//Note: other uniforms may have been set from the ui at this point.
	//Other uniforms could be set here
    shader->setUniformValue( "light.intensity", QVector3D( 1.0f, 1.0f, 1.0f ) );
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

	//TODO: remove in simple example?!
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
	QVector3D vWorld;
	//Transform local movement to worldspace.
    vWorld += vLocal.z() * m_cameraToCenter.normalized();
	if(m_cameraMode == CAMERMODE_WALKTHROUGH)
	{
		// Use crossproduct of camera front direction and up vector for sideway movement
		QVector3D x = QVector3D::crossProduct( m_cameraToCenter, m_upVector ).normalized();
		vWorld += vLocal.x() * x;
		// Use upvector for upward movement
		vWorld += vLocal.y() * m_upVector;
		// update the view center coordinates when using a walkthrough camera
		m_viewCenter += vWorld;
	} else {
		// update the cameraToCenter vector (only in z direction) when using objectInspection
		m_cameraToCenter -= vWorld;
	}
	// Update the camera position
	m_position += vWorld;
}

// pan (point camera right or left)
void ShaderTestScene::pan( const float &angle )
{
	//rotate the up- and cameraToCenter-vector using quaternion-math.
	QQuaternion q = QQuaternion::fromAxisAndAngle( QVector3D(0.0f,1.0f,0.0f), -angle );
	//If the camera looks up or down, the upvector rotates while panning
	m_upVector = q.rotatedVector( m_upVector );
	m_cameraToCenter = q.rotatedVector( m_cameraToCenter );
	if(m_cameraMode == CAMERMODE_WALKTHROUGH)
	{
		m_viewCenter = m_position + m_cameraToCenter;
	} else {
		m_position = m_viewCenter - m_cameraToCenter;
	}
}

//tilt (point camera up and down)
void ShaderTestScene::tilt( const float &angle )
{
	QVector3D xBasis = QVector3D::crossProduct( m_upVector, m_cameraToCenter.normalized() ).normalized();
    QQuaternion q = QQuaternion::fromAxisAndAngle( xBasis, angle );
    m_upVector = q.rotatedVector( m_upVector );
    m_cameraToCenter = q.rotatedVector( m_cameraToCenter );
	if(m_cameraMode == CAMERMODE_WALKTHROUGH)
	{
		m_viewCenter = m_position + m_cameraToCenter;
	} else {
		m_position = m_viewCenter - m_cameraToCenter;
	}
}

void ShaderTestScene::recompileShader()
{
	prepareShaders();
	prepareTextures();
	//TODO: resize is only needed for viewPortmatrix at the moment. TODO: Add uniform functions for matrices...
	resize(m_viewportSize.x(), m_viewportSize.y());
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
	QString vertexShader;
	QString tessControlShader;
	QString tessEvalShader;
	QString geometryShader;
	QString fragmentShader;
	if(m_shaderInfo.tesselationControlShaderFile.isEmpty())
	{
		if(m_shaderInfo.geometryShaderFile.isEmpty())
		{
			m_material->setShaders( m_shaderInfo.vertexShaderFile, m_shaderInfo.fragmentShaderFile);
		}
		else
		{
			m_material->setShaders( m_shaderInfo.vertexShaderFile, m_shaderInfo.geometryShaderFile, m_shaderInfo.fragmentShaderFile);
		}
	}
	else
	{
		m_material->setShaders( m_shaderInfo.vertexShaderFile, m_shaderInfo.tesselationControlShaderFile, m_shaderInfo.tesselationEvaluationShaderFile, m_shaderInfo.geometryShaderFile, m_shaderInfo.fragmentShaderFile);
	}

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
	QHashIterator<QString, bool> iter1b(m_initialUniforms1b);
	while (iter1b.hasNext()) {
		iter1b.next();
		setShaderUniformValue(iter1b.key().toStdString().c_str(), iter1b.value());
	}
}

void ShaderTestScene::prepareTextures()
{
	if(m_tilingSamplerId != 0) {
		m_funcs->glDeleteSamplers( 1, &m_tilingSamplerId );
	}
    m_funcs->glGenSamplers( 1, &m_tilingSamplerId );
	m_funcs->glSamplerParameteri( m_tilingSamplerId, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    m_funcs->glSamplerParameterf( m_tilingSamplerId, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f );
	m_funcs->glSamplerParameteri( m_tilingSamplerId, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	m_funcs->glSamplerParameteri( m_tilingSamplerId, GL_TEXTURE_WRAP_S, GL_REPEAT );
	m_funcs->glSamplerParameteri( m_tilingSamplerId, GL_TEXTURE_WRAP_T, GL_REPEAT );

	QImage diffuseImage( "./resources/textures/Nobiax Free Textures 13 and 14/pattern_69/diffus.png" );
    m_funcs->glActiveTexture( GL_TEXTURE0 );
    TexturePtr diffuseTexture( new Texture );
    diffuseTexture->create();
    diffuseTexture->bind();
    diffuseTexture->setImage( diffuseImage );
    diffuseTexture->generateMipMaps();
    m_material->setTextureUnitConfiguration( 1, diffuseTexture, m_tilingSamplerId, QByteArrayLiteral( "diffuseTexture" ) );

	QImage heightImage( "./resources/textures/Nobiax Free Textures 13 and 14/pattern_69/height.png" );
    m_funcs->glActiveTexture( GL_TEXTURE1 );
    TexturePtr heightTexture( new Texture );
    heightTexture->create();
    heightTexture->bind();
    heightTexture->setImage( heightImage );
    heightTexture->generateMipMaps();
    m_material->setTextureUnitConfiguration( 2, heightTexture, m_tilingSamplerId, QByteArrayLiteral( "heightTexture" ) );

	QImage normalImage( "./resources/textures/Nobiax Free Textures 13 and 14/pattern_69/normal.png" );
    m_funcs->glActiveTexture( GL_TEXTURE2 );
    TexturePtr normalTexture( new Texture );
    normalTexture->create();
    normalTexture->bind();
    normalTexture->setImage( normalImage );
    normalTexture->generateMipMaps();
    m_material->setTextureUnitConfiguration( 3, normalTexture, m_tilingSamplerId, QByteArrayLiteral( "normalTexture" ) );

	QImage specularImage( "./resources/textures/Nobiax Free Textures 13 and 14/pattern_69/specular.png" );
    m_funcs->glActiveTexture( GL_TEXTURE3 );
    TexturePtr specularTexture( new Texture );
    specularTexture->create();
    specularTexture->bind();
    specularTexture->setImage( specularImage );
    specularTexture->generateMipMaps();
    m_material->setTextureUnitConfiguration( 4, specularTexture, m_tilingSamplerId, QByteArrayLiteral( "specularTexture" ) );

	QImage displaceImage( "./resources/textures/Nobiax Free Textures 13 and 14/pattern_69/displacement.png" );
    m_funcs->glActiveTexture( GL_TEXTURE4 );
    TexturePtr displaceTexture(new Texture );
    displaceTexture->create();
    displaceTexture->bind();
    displaceTexture->setImage( displaceImage );
    displaceTexture->generateMipMaps();
    m_material->setTextureUnitConfiguration( 5, displaceTexture, m_tilingSamplerId, QByteArrayLiteral( "displaceTexture" ) );

    m_funcs->glActiveTexture( GL_TEXTURE0 );
}

void ShaderTestScene::prepareVertexBuffers()
{
	LoaderObj loader = LoaderObj("resources/objects/bunny.obj");
	//LoaderObj loader = LoaderObj("resources/objects/san-miguel.obj");
	//LoaderObj loader = LoaderObj("resources/objects/crytek-sponza/sponza.obj");
	m_elementCount = loader.getIndexCount();
	m_vertexCount = loader.getVertexCount();

    m_positionBuffer.create();
    m_positionBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_positionBuffer.bind();
	GLfloat *pVB = loader.getVB();
	m_positionBuffer.allocate( pVB, m_vertexCount*3*sizeof(GLfloat) );
    m_positionBuffer.release();
	
    m_normalsBuffer.create();
    m_normalsBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_normalsBuffer.bind();
	GLfloat *pB = loader.getCalculatedNormals();
	m_normalsBuffer.allocate(pB, loader.getVertexCount()*3*sizeof(GLfloat) ); //TODO: Kommentar (GPU)
    m_normalsBuffer.release();
	
	m_tangentsBuffer.create();
    m_tangentsBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_tangentsBuffer.bind();
	GLfloat *pT = loader.getCalculatedTangents();
	m_tangentsBuffer.allocate(pT, loader.getVertexCount()*3*sizeof(GLfloat) );
    m_tangentsBuffer.release();
	
	m_bitangentsBuffer.create();
    m_bitangentsBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_bitangentsBuffer.bind();
	GLfloat *pBT = loader.getCalculatedBitangents();
	m_bitangentsBuffer.allocate(pBT, loader.getVertexCount()*3*sizeof(GLfloat) );
    m_bitangentsBuffer.release();
	
    m_texCoordsBuffer.create();
    m_texCoordsBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_texCoordsBuffer.bind();
	GLfloat *pTC = loader.getCalculatedTexCoords();
	m_texCoordsBuffer.allocate(pTC, loader.getVertexCount()*2*sizeof(GLfloat) );
    m_texCoordsBuffer.release();
	
    m_indexBuffer.create();
    m_indexBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_indexBuffer.bind();
	GLuint *pIB = loader.getIB();
	m_indexBuffer.allocate( pIB, loader.getIndexCount()*sizeof(GLuint) );
    m_indexBuffer.release();

    m_vaoBunny.create();
    {
        QOpenGLVertexArrayObject::Binder binder( &m_vaoBunny );
		QOpenGLShaderProgramPtr shader = m_material->shader();
        shader->bind();

        m_positionBuffer.bind();
		shader->setAttributeBuffer("in_Position", GL_FLOAT, 0, 3, 3*sizeof(float));
        shader->enableAttributeArray( "in_Position" );

        m_normalsBuffer.bind();
		shader->setAttributeBuffer("in_Normal", GL_FLOAT, 0, 3, 3*sizeof(float));
        shader->enableAttributeArray( "in_Normal" );

        m_tangentsBuffer.bind();
		shader->setAttributeBuffer("in_Tangent", GL_FLOAT, 0, 3, 3*sizeof(float));
        shader->enableAttributeArray( "in_Tangent" );

        m_bitangentsBuffer.bind();
		shader->setAttributeBuffer("in_Bitangent", GL_FLOAT, 0, 3, 3*sizeof(float));
        shader->enableAttributeArray( "in_Bitangent" );

        m_texCoordsBuffer.bind();
		shader->setAttributeBuffer("in_TexCoords", GL_FLOAT, 0, 2, 2*sizeof(float));
        shader->enableAttributeArray( "in_TexCoords" );

		m_indexBuffer.bind();
    }
	//TODO: think over these release()s
	//After using the vao, resources must be released
 //   m_material->shader()->release();
 //   m_positionBuffer.release();
 //   m_normalsBuffer.release();
	//m_tangentsBuffer.release();
	//m_bitangentsBuffer.release();
 //   m_texCoordsBuffer.release();
 //   m_indexBuffer.release();


	const float planeSizeX = 1.f;
	const float planeSizeZ = 1.f;
	const float planeTexRepeatX = 1.f;
	const float planeTexRepeatZ = 1.f;

	float *planeVertexPositions = new float[m_planeVertexCount*3];
	float *planeVertexNormals = new float[m_planeVertexCount*3];
	float *planeVertexTangents = new float[m_planeVertexCount*3];
	float *planeVertexBitangents = new float[m_planeVertexCount*3];
	float *planeVertexTexCoords = new float[m_planeVertexCount*2];
	int *planeIndices = new int[m_planeElementCount]; 
	
	const float texFactorX = 1.f/(float)m_planeResolutionX*planeTexRepeatX;
	const float texFactorZ = 1.f/(float)m_planeResolutionZ*planeTexRepeatZ;
	const float posFactorX = 1.f/(float)m_planeResolutionX*planeSizeX;
	const float posFactorZ = 1.f/(float)m_planeResolutionZ*planeSizeZ;
	for(unsigned int ix=0 ; ix<m_planeResolutionX ; ++ix)
	{
		float posX = ((float)ix)*posFactorX-(planeSizeX*0.5);
		float texX = ((float)ix)*texFactorX;
		for(unsigned int iz=0 ; iz<m_planeResolutionZ ; ++iz)
		{
			unsigned int idx = ix*m_planeResolutionX+iz;
			unsigned int idxx3 = idx*3;
			
			if(idx*2+1>=m_planeVertexCount*2)
			{
				qCritical() << "Array access out of bounds";
				exit(1);
			}
			if(idxx3+2>=m_planeVertexCount*3)
			{
				qCritical() << "Array access out of bounds";
				exit(1);
			}
			planeVertexPositions[idxx3] = posX;
			planeVertexPositions[idxx3+1] = 0.f;
			planeVertexPositions[idxx3+2] = ((float)iz)*posFactorZ-(planeSizeZ*0.5);

			planeVertexNormals[idxx3] =   0.f;
			planeVertexNormals[idxx3+1] = 1.f;
			planeVertexNormals[idxx3+2] = 0.f;

			planeVertexTangents[idxx3] =   1.f;
			planeVertexTangents[idxx3+1] = 0.f;
			planeVertexTangents[idxx3+2] = 0.f;

			planeVertexBitangents[idxx3] =   0.f;
			planeVertexBitangents[idxx3+1] = 0.f;
			planeVertexBitangents[idxx3+2] = 1.f;

			planeVertexTexCoords[idx*2] =   texX;
			planeVertexTexCoords[idx*2+1] = ((float)iz)*texFactorZ;
		}
	}
	for(unsigned int ix=0 ; ix<m_planeResolutionX-1 ; ++ix)
	{
		for(unsigned int iz=0 ; iz<m_planeResolutionZ-1 ; ++iz)
		{
			//construct indices for two counterclockwise triangles
			unsigned int idxIndex = (ix*(m_planeResolutionX-1)+iz)*3*2;
			unsigned int idxVert = ix*m_planeResolutionX+iz;
			
			if(idxIndex+5>=m_planeElementCount)
			{
				qCritical() << "Array access out of bounds";
				exit(1);
			}
			planeIndices[idxIndex] =   idxVert;
			planeIndices[idxIndex+1] = idxVert + 1;
			planeIndices[idxIndex+2] = idxVert + 1 + m_planeResolutionX;

			planeIndices[idxIndex+3] = idxVert;
			planeIndices[idxIndex+4] = idxVert + 1 + m_planeResolutionX;
			planeIndices[idxIndex+5] = idxVert + m_planeResolutionX;
		}
	}
	//The same for a plane object

    m_planePositionBuffer.create();
    m_planePositionBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_planePositionBuffer.bind();
	m_planePositionBuffer.allocate( planeVertexPositions, m_planeVertexCount*3*sizeof(GLfloat) ); // 4 Vertices for the plane, 3 components per vertex (x,y,z)
    m_planePositionBuffer.release();
	
    m_planeIndexBuffer.create();
    m_planeIndexBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_planeIndexBuffer.bind();
	m_planeIndexBuffer.allocate( planeIndices, m_planeElementCount*sizeof(GLuint) ); // 2Faces*3VertexIndices
    m_planeIndexBuffer.release();

    m_planeNormalsBuffer.create();
    m_planeNormalsBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_planeNormalsBuffer.bind();
	m_planeNormalsBuffer.allocate(planeVertexNormals, m_planeVertexCount*3*sizeof(GLfloat) ); // 4 Vertices for the plane, 3 components per vertex (x,y,z)
    m_planeNormalsBuffer.release();

    m_planeTangentsBuffer.create();
    m_planeTangentsBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_planeTangentsBuffer.bind();
	m_planeTangentsBuffer.allocate(planeVertexTangents, m_planeVertexCount*3*sizeof(GLfloat) ); // 4 Vertices for the plane, 3 components per vertex (x,y,z)
    m_planeTangentsBuffer.release();

    m_planeBitangentsBuffer.create();
    m_planeBitangentsBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_planeBitangentsBuffer.bind();
	m_planeBitangentsBuffer.allocate(planeVertexBitangents, m_planeVertexCount*3*sizeof(GLfloat) ); // 4 Vertices for the plane, 3 components per vertex (x,y,z)
    m_planeBitangentsBuffer.release();

    m_planeTexCoordsBuffer.create();
    m_planeTexCoordsBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_planeTexCoordsBuffer.bind();
	m_planeTexCoordsBuffer.allocate(planeVertexTexCoords, m_planeVertexCount*2*sizeof(GLfloat) ); // 4 Vertices for the plane, 2 components per vertex (u, v)
    m_planeTexCoordsBuffer.release();
	
	delete [] planeVertexPositions;
	delete [] planeVertexNormals;
	delete [] planeVertexTangents;
	delete [] planeVertexBitangents;
	delete [] planeVertexTexCoords;
	delete [] planeIndices; 

    m_vaoPlane.create();
    {
        QOpenGLVertexArrayObject::Binder binder( &m_vaoPlane );
		QOpenGLShaderProgramPtr shader = m_material->shader();
        shader->bind();
        m_planePositionBuffer.bind();
		shader->setAttributeBuffer("in_Position", GL_FLOAT, 0, 3, 3*sizeof(float));
        shader->enableAttributeArray( "in_Position" );

        m_planeNormalsBuffer.bind();
		shader->setAttributeBuffer("in_Normal", GL_FLOAT, 0, 3, 3*sizeof(float));
        shader->enableAttributeArray( "in_Normal" );

        m_planeTangentsBuffer.bind();
		shader->setAttributeBuffer("in_Tangent", GL_FLOAT, 0, 3, 3*sizeof(float));
        shader->enableAttributeArray( "in_Tangent" );

        m_planeBitangentsBuffer.bind();
		shader->setAttributeBuffer("in_Bitangent", GL_FLOAT, 0, 3, 3*sizeof(float));
        shader->enableAttributeArray( "in_Bitangent" );

        m_planeTexCoordsBuffer.bind();
		shader->setAttributeBuffer("in_TexCoords", GL_FLOAT, 0, 2, 2*sizeof(float));
        shader->enableAttributeArray( "in_TexCoords" );
		m_planeIndexBuffer.bind();
    }

	//m_material->shader()->release();
	//m_planePositionBuffer.release();
	//m_planeNormalsBuffer.release();
	//m_planeTangentsBuffer.release();
	//m_planeBitangentsBuffer.release();
	//m_planeTexCoordsBuffer.release();
	//m_planeIndexBuffer.release();

	//releasing the objects internally cause glBindXXX(0).
	// we have to do this, because QtQuick wants a clean OpenGL state
	m_funcs->glBindBuffer(GL_ARRAY_BUFFER, 0);
	m_funcs->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	m_funcs->glUseProgram(0);
}

void ShaderTestScene::setShaderUniformValue(const char *name, const float &val)
{
	m_initialUniforms1f[QString(name)] = val;
	if(!m_isInitialized)
	{
		return;
	}
	qDebug() << "Set Uniform " << name << ": " << val;
	QOpenGLShaderProgramPtr shader = m_material->shader();
	shader->bind();
	shader->setUniformValue( name, val );
	shader->release();
}

void ShaderTestScene::setShaderUniformValue(const char *name, const GLint &val)
{
	m_initialUniforms1i[QString(name)] = val;
	if(!m_isInitialized)
	{
		return;
	}
	qDebug() << "Set Uniform " << name << ": " << val;
	QOpenGLShaderProgramPtr shader = m_material->shader();
	shader->bind();
	shader->setUniformValue( name, val );
	shader->release();
}

void ShaderTestScene::setShaderUniformValue(const char *name, const float &x, const float &y, const float &z)
{
	m_initialUniforms3f[QString(name)] = QVector3D(x,y,z);
	if(!m_isInitialized)
	{
		return;
	}
	qDebug() << "Set Uniform " << name << ": (" << x << ", " << y << ", " << z << ")";
	QOpenGLShaderProgramPtr shader = m_material->shader();
	shader->bind();
	shader->setUniformValue( name, QVector3D( x, y, z ) );
	shader->release();
}

void ShaderTestScene::setShaderUniformValue(const char *name, const bool &val)
{
	m_initialUniforms1b[QString(name)] = val;
	if(!m_isInitialized)
	{
		return;
	}
	qDebug() << "Set Uniform " << name << ": " << val;
	QOpenGLShaderProgramPtr shader = m_material->shader();
	shader->bind();
	shader->setUniformValue( name, val );
	shader->release();
}

void ShaderTestScene::setActiveShader(const ShaderInfo &shader)
{
	m_shaderInfo = shader;
	if(m_isInitialized)
	{
		recompileShader();
	}
}

void ShaderTestScene::setSelectedMinFilter(const GLuint &minFilter)
{
	if(m_isInitialized && m_tilingSamplerId)
	{
		m_funcs->glSamplerParameteri( m_tilingSamplerId, GL_TEXTURE_MIN_FILTER, minFilter );
	}
}
void ShaderTestScene::setSelectedMagFilter(const GLuint &magFilter)
{
	if(m_isInitialized && m_tilingSamplerId)
	{
		m_funcs->glSamplerParameteri( m_tilingSamplerId, GL_TEXTURE_MAG_FILTER, magFilter );
	}
}
void ShaderTestScene::setAnisotropy(const GLfloat &anisotropy)
{
	if(m_isInitialized && m_tilingSamplerId && anisotropy != 0)
	{
		m_funcs->glSamplerParameterf( m_tilingSamplerId, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy );
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