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

#include <QOpenGLFunctions_4_2_Core>

// Enabling Qt debug features will prevent nvidia nsight debugging tool from working. This is likely to be fixed in the future
#define DEBUG_OPENGL_ENABLED 1

class Camera;
class QQuickItem;

class QOpenGLFunctions_4_2_Core;


class ShaderTestScene : public AbstractScene
{
    Q_OBJECT

public:
    ShaderTestScene( QObject* parent = 0 );

    virtual void initialise();
    virtual void update( float t );
    virtual void render();
    virtual void resize( int w, int h );

	virtual void setShaderUniformValue(const char *name,const float &val);
	virtual void setShaderUniformValue(const char *name,const int &val);
	virtual void setShaderUniformValue(const char *name,const float &x, const float &y, const float &z);
	
	virtual void setCamerModeWalkthrough() { m_cameraMode = CAMERMODE_WALKTHROUGH;}
	virtual void setCamerModeObjectInspection() { m_cameraMode = CAMERMODE_OBJECTINSPECTION;}

    // Camera motion control
    void setSideSpeed( float vx ) { m_v.setX( vx ); }
    void setVerticalSpeed( float vy ) { m_v.setY( vy ); }
    void setForwardSpeed( float vz ) { m_v.setZ( vz ); }
    void setViewCenterFixed( bool b ) { m_viewCenterFixed = b; }

    // Camera orientation control
    void pan( float angle ) { m_panAngle = angle; }
    void tilt( float angle ) { m_tiltAngle = angle; }

    enum DisplayMode {
        SimpleWireFrame = 0,
        WorldHeight,
        WorldNormals,
        Grass,
        GrassAndRocks,
        GrassRocksAndSnow,
        LightingFactors,
        TexturedAndLit,
        DisplayModeCount
    };

	enum CameraMode {
		CAMERMODE_WALKTHROUGH,
		CAMERMODE_OBJECTINSPECTION
	};

    void setDisplayMode( DisplayMode displayMode ) { m_displayMode = displayMode; }
    DisplayMode displayMode() const { return m_displayMode; }

	void recompileShader();

	unsigned int m_glCullMode;
public slots:
	virtual void setActiveShader(const ShaderInfo &shader);

private:
    void prepareShaders();
    void prepareTextures();
    void prepareVertexBuffers();

	void genNormalsGPU();

	bool m_isInitialized;
	ShaderInfo m_shaderInfo;
	QOpenGLDebugLogger m_logger;
    Camera* m_camera;
    QVector3D m_v;
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

    DisplayMode m_displayMode;
    QStringList m_displayModeNames;
    QVector<GLuint> m_displayModeSubroutines;

    QOpenGLFunctions_4_2_Core* m_funcs;
private slots:
	void onMessageLogged( QOpenGLDebugMessage message );
};

#endif // SHADERTESTSCENE_H
