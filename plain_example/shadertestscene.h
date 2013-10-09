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

class ShaderTestScene : public AbstractScene
{
    Q_OBJECT

public:
    ShaderTestScene( QObject* parent = 0 );

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

    // Camera motion control
    void setSideSpeed( float vx ) { m_v.setX( vx ); }
    void setVerticalSpeed( float vy ) { m_v.setY( vy ); }
    void setForwardSpeed( float vz ) { m_v.setZ( vz ); }
    void setViewCenterFixed( bool b ) { m_viewCenterFixed = b; }
    void moveForward( float vz ) { m_offset.setZ( m_offset.z() + vz ); }


	enum CameraMode {
		CAMERMODE_WALKTHROUGH,
		CAMERMODE_OBJECTINSPECTION
	};

    // Translate relative to camera orientation axes
    void translate( const QVector3D& vLocal);

    // Camera orientation control
    void pan( const float &angle );
    void tilt( const float &angle );

	void recompileShader();

	void setGlCullMode(int cullMode) { m_glCullMode = cullMode; }
	void setRotationSpeed(float rot) { m_rotationSpeed = rot; }

public slots:
	void setActiveShader(const ShaderInfo &shader);

private:
    void prepareShaders();
    void prepareTextures();
    void prepareVertexBuffers();
	void passUniforms();

	void genNormalsGPU();

	unsigned int m_glCullMode;
	unsigned int m_rotationSpeed;

	QHash<QString, float> m_initialUniforms1f;
	QHash<QString, int> m_initialUniforms1i;
	QHash<QString, QVector3D> m_initialUniforms3f;
	QHash<QString, bool> m_initialUniforms1b;

	////Vectors and Matrices, Transformation related////
    QVector3D m_position;
    QVector3D m_upVector;
    QVector3D m_viewCenter;

    QVector3D m_cameraToCenter; // The vector from the camera position to the view center

    QMatrix4x4 m_viewMatrix;
    QMatrix4x4 m_projectionMatrix;
    QMatrix4x4 m_viewProjectionMatrix;

	bool m_isInitialized;
	ShaderInfo m_shaderInfo;
	QOpenGLDebugLogger m_logger;

    QVector3D m_v;
    QVector3D m_offset;
    bool m_viewCenterFixed;
    float m_panAngle;
    float m_tiltAngle;

	CameraMode m_cameraMode;
	//Note: we must not use a shared pointer here, as Qt would try to delete the element twice when the application exits.
	//QQuickItem* m_rootObject;
	
    QMatrix4x4 m_viewportMatrix;
    QVector2D m_viewportSize;

    QOpenGLVertexArrayObject m_vao;
	QOpenGLBuffer m_vertexBuffer;
	QOpenGLBuffer m_indexBuffer;

	QOpenGLBuffer m_normalsBuffer;

    unsigned int  m_elementCount;
	unsigned int m_vertexCount;

    float m_screenSpaceError;
	
	//Note: we must not use a shared pointer here, as Qt would try to delete the element twice when the application exits.
    Material* m_material;

    QMatrix4x4 m_modelMatrix;

    float m_time;
    const float m_metersToUnits;

    QStringList m_displayModeNames;
    QVector<GLuint> m_displayModeSubroutines;

    QOpenGLFunctions_4_2_Core* m_funcs;
private slots:
	void onMessageLogged( QOpenGLDebugMessage message );
};

#endif // SHADERTESTSCENE_H
