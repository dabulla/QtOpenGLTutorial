#ifndef OPENGLTEST_H
#define OPENGLTEST_H

#include <QWindow>
#include <QOpenGLFunctions_4_0_Core>
#include <qopenglfunctions_4_3_core>
#include <qopengl.h>
#include <qopenglbuffer.h>
#include <qopenglcontext.h>
#include <qopenglvertexarrayobject.h>
#include <qopengldebug.h>
#include <qopenglshaderprogram.h>
#include <QOpenGLDebugLogger>
#include "ui_opengltest.h"
#include "camera.h"

class OpenGLTest : public QWindow
{
	Q_OBJECT

public:
	OpenGLTest(QScreen *screen = 0);
	~OpenGLTest();
protected:
	QOpenGLContext *m_context;
	QOpenGLFunctions_4_3_Core *m_funcs;
	QOpenGLVertexArrayObject *m_vao1;
	QOpenGLBuffer m_positionBuffer;
	QOpenGLBuffer m_indexBuffer;
	QOpenGLShaderProgram m_shaderProgram;
	QOpenGLDebugLogger m_logger;
	Camera m_camera;
	void initialize();
	QVector2D m_viewportSize;
	QMatrix4x4 m_viewportMatrix;
private slots:
	void render();
	void onMessageLogged( QOpenGLDebugMessage message );
	void resize();
};

#endif // OPENGLTEST_H
