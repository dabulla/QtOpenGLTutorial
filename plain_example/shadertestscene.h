#ifndef SHADERTESTSCENE_H
#define SHADERTESTSCENE_H

#include "abstractscene.h"
#include "material.h"

#include <QOpenGLBuffer>
#include <QOpenGLDebugLogger>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>
#include <QStringList>
#include <qhash.h>

#include <QOpenGLFunctions_4_2_Core>

// Enabling Qt debug features will prevent nvidia nsight debugging tool from working. This is likely to be fixed in the future
#define DEBUG_OPENGL_ENABLED

class Camera;
class QQuickItem;

class QOpenGLFunctions_4_2_Core;

//ShaderInfo: Contains Info about the ShaderProcedure that can be used.
// Two files can be specified the add a fragment and vertex shader.
// moreover a subroutine can be specified. This way different shaders can be defined in one file.
// This has the advantage, that common function in the file can be reused: e.g. phong(...) can be used for
// a plain phong shader and a textured phong shader
struct ShaderInfo
{
	QString vertexShaderFile;
	QString vertexShaderProc;
	QString tesselationControlShaderFile;
	QString tesselationControlShaderProc;
	QString tesselationEvaluationShaderFile;
	QString tesselationEvaluationShaderProc;
	QString geometryShaderFile;
	QString geometryShaderProc;
	QString fragmentShaderFile;
	QString fragmentShaderProc;
};

class ShaderTestScene : public QObject
{
    Q_OBJECT
		
public:
    ShaderTestScene( QObject* parent = 0 );
	ShaderTestScene::~ShaderTestScene();

    virtual void initialise();
    virtual void update( float t );
    virtual void render();
    virtual void resize( int w, int h );

	virtual void setShaderUniformValue(const char *name, const float &val);
	virtual void setShaderUniformValue(const char *name, const int &val);
	virtual void setShaderUniformValue(const char *name, const float &x, const float &y, const float &z);
	virtual void setShaderUniformValue(const char *name, const bool &val);
	
	virtual void setCamerModeWalkthrough() { m_cameraMode = CAMERMODE_WALKTHROUGH;}
	virtual void setCamerModeObjectInspection() { m_cameraMode = CAMERMODE_OBJECTINSPECTION;}
	
	virtual void setObjectBunny() { m_currentObject = OBJECT_BUNNY;}
	virtual void setObjectPlane() { m_currentObject = OBJECT_QUAD;}

    // Camera motion control
    void moveSide( float x ) { m_offset.setX( m_offset.x() + x ); }
    void moveVertical( float y ) { m_offset.setY( m_offset.y() + y ); }
    void moveForward( float z ) { m_offset.setZ( m_offset.z() + z ); }

	// Camera motion control
	// This is needed to set movement-speed independent of frames.
	// OnKeydown -> set movementspeed 1
	// OnKeyUp -> set movementspeed 0
    void setMovementSide( float vx ) { m_velocity.setX( vx ); }
    void setMovementVertical( float vy ) { m_velocity.setY( vy ); }
    void setMovementForward( float vz ) { m_velocity.setZ( vz ); }
	
	enum CameraMode {
		CAMERMODE_WALKTHROUGH,
		CAMERMODE_OBJECTINSPECTION
	};

	enum CurrentObject {
		OBJECT_BUNNY,
		OBJECT_QUAD
	};


    // Camera orientation control
    void pan( const float &angle );
    void tilt( const float &angle );

	void recompileShader();

	void setGlCullMode(int cullMode) { m_glCullMode = cullMode; }
	void setRotationSpeed(float rot) { m_rotationSpeed = rot; }
	
    void setContext( QOpenGLContext* context ) { m_context = context; }
    QOpenGLContext* context() const { return m_context; }
public slots:
	void setActiveShader(const ShaderInfo &shader);
	void setSelectedMinFilter(const GLuint &minFilter);
	void setSelectedMagFilter(const GLuint &magFilter);
	void setAnisotropy(const GLfloat &anisotropy);

private:
    void prepareShaders();
    void prepareTextures();
    void prepareVertexBuffers();
	void passUniforms();

	//void genNormalsGPU();
	
    // Translate relative to camera orientation axes
    void translate( const QVector3D& vLocal);
	
	unsigned int m_glCullMode;

	QHash<QString, float> m_initialUniforms1f;
	QHash<QString, int> m_initialUniforms1i;
	QHash<QString, QVector3D> m_initialUniforms3f;
	QHash<QString, bool> m_initialUniforms1b;


	////Vectors and Matrices, Transformation related////
    QVector3D m_position;
    QVector3D m_upVector;
    QVector3D m_viewCenter;

    QVector3D m_cameraToCenter; // The vector from the camera position to the view center

    QVector3D m_offset; //Used to accumulate movement during frame.
    QVector3D m_velocity;

    QMatrix4x4 m_viewMatrix;
    QMatrix4x4 m_projectionMatrix;
    QMatrix4x4 m_viewProjectionMatrix;
    QMatrix4x4 m_modelMatrix;
	
    float m_panAngle;
    float m_tiltAngle;

	CameraMode m_cameraMode;

	CurrentObject m_currentObject;
	
    QMatrix4x4 m_viewportMatrix;
    QVector2D m_viewportSize;

	GLuint	m_tilingSamplerId;

	/// All members used for bunny object
    QOpenGLVertexArrayObject m_vaoBunny;

	QOpenGLBuffer m_positionBuffer;
	QOpenGLBuffer m_normalsBuffer;
	QOpenGLBuffer m_tangentsBuffer;
	QOpenGLBuffer m_bitangentsBuffer;
	QOpenGLBuffer m_texCoordsBuffer;
	QOpenGLBuffer m_indexBuffer;

    unsigned int  m_elementCount;
	unsigned int m_vertexCount;
	
	// All members used for plane object
	const unsigned int m_planeResolutionX = 8;
	const unsigned int m_planeResolutionZ = 8;

    QOpenGLVertexArrayObject m_vaoQuad;

	QOpenGLBuffer m_quadPositionBuffer;
	QOpenGLBuffer m_quadNormalsBuffer;
	QOpenGLBuffer m_quadTangentsBuffer;
	QOpenGLBuffer m_quadBitangentsBuffer;
	QOpenGLBuffer m_quadTexCoordsBuffer;
	QOpenGLBuffer m_quadIndexBuffer;
	
    const unsigned int  m_quadElementCount = m_planeResolutionX*m_planeResolutionZ;
	const unsigned int m_quadVertexCount = (m_planeResolutionX-1)*(m_planeResolutionZ-1)*2; // For each Vertex there will be two triangles, except the last row and column of the plane.

	// Not used yet. This would be important for adaptive tesselation
    float m_screenSpaceError;
	
    Material* m_material;

    float m_time;
    const float m_metersToUnits;
	
	bool m_isInitialized;
	ShaderInfo m_shaderInfo;
	QOpenGLDebugLogger m_logger;

	unsigned int m_rotationSpeed;

    QOpenGLContext* m_context;
    QOpenGLFunctions_4_2_Core* m_funcs;
private slots:
	void onMessageLogged( QOpenGLDebugMessage message );
};

#endif // SHADERTESTSCENE_H
