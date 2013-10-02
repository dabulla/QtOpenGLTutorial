#ifndef ABSTRACTSCENE_H
#define ABSTRACTSCENE_H

#include <QObject>

class QOpenGLContext;
class QQuickItem;

//ShaderInfo: Contains Info about the ShaderProcedure that can be used.
// Two files can be specified the add a fragment and vertex shader.
// moreover a subroutine can be specified. This way different shaders can be defined in one file.
// This has the advantage, that common function in the file can be reused: e.g. phong(...) can be used for
// a plain phong shader and a textured phong shader
struct ShaderInfo
{
	QString vertexShaderFile;
	QString vertexShaderProc;
	QString fragmentShaderFile;
	QString fragmentShaderProc;
};

class AbstractScene : public QObject
{
    Q_OBJECT

public:
    AbstractScene( QObject* parent = 0 );

    void setContext( QOpenGLContext* context ) { m_context = context; }
    QOpenGLContext* context() const { return m_context; }

    /**
      Load textures, initialize shaders, etc.
      */
    virtual void initialise() = 0;

    /**
      This is called prior to every frame.  Use this
      to update your animation.
      */
    virtual void update( float t ) = 0;

    /**
      Draw your scene.
      */
    virtual void render() = 0;

    /**
      Called when screen is resized
      */
    virtual void resize( int w, int h ) = 0;

	/**
		Set a Shader uniform. The concrete scene must be able to call the correct glUniformxxx variable.
		// eg. 
		//TODO: datatypes for uniforms
	*/
	//virtual void setShaderUniformValue(const char *name, const float &val) = 0;
	//virtual void setShaderUniformValue(const char *name, const int &val) = 0;
	//virtual void setShaderUniformValue(const char *name, const float &x, const float &y, const float &z) = 0;
	//
	//virtual void setCamerModeWalkthrough() = 0;
	//virtual void setCamerModeObjectInspection() = 0;

public slots:
	virtual void setActiveShader(const ShaderInfo &) = 0;
protected:
    QOpenGLContext* m_context;
};

#endif // ABSTRACTSCENE_H
