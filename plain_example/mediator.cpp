#include "mediator.h"
#include "shadertestscene.h"
#include "window.h"
#include <qquickitem.h>
#include <qqmlcontext.h>
#include <qsurface.h>

Mediator::Mediator(QObject *parent, ShaderTestScene *scene, Window *w)
	: QObject(parent),
	m_mainWnd(w),
	m_scene(scene),
	m_uiWnd(0)
{
	//Register the Mediator Class as a type usable in qml. This makes Q_PROPERTY work (setter, gett, propertychange...)
	// Calling functions from qml works without this registration.
	// we use "qmlRegisterInterface" instead of "qmlRegisterType" because we do not provide a standard constructor
	// and do not want to instantiate the class in qml (We actually only spawn one object named "application" from window.cpp to qml).
	// http://qt-project.org/doc/qt-5.0/qtqml/qtqml-cppintegration-definetypes.html
	qmlRegisterUncreatableType<Mediator>("de.fhaachen", 1, 0, "ShaderTestSceneMediator", "Use the provided \"application\" object to communicate with cpp");
}

Mediator::~Mediator()
{
}

Q_INVOKABLE void Mediator::reloadShader()
{
	m_scene->recompileShader();
}

Q_INVOKABLE void Mediator::toggleDialog()
{
	if(	m_uiWnd == 0)
	{
		m_uiWnd = new QQuickView(m_mainWnd);
		m_uiWnd->setSource(m_mainWnd->m_sourcePath);
		m_uiWnd->setResizeMode(QQuickView::SizeRootObjectToView);
		m_uiWnd->setWidth(200);
		m_uiWnd->setHeight(500);
		m_uiWnd->setSurfaceType( QSurface::OpenGLSurface );
		m_uiWnd->rootContext()->setContextProperty("application", this);
		m_uiWnd->setFlags(Qt::Popup | Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
		m_uiWnd->create();
		m_uiWnd->show();
		m_mainWnd->rootObject()->setOpacity(0);
	}
	else
	{
		m_uiWnd->destroy();
		m_uiWnd = 0;
		m_mainWnd->rootObject()->setOpacity(1);
	}
}

Q_INVOKABLE void Mediator::setShaderUniformValue(const char *name, const float &val)
{
	m_scene->setShaderUniformValue(name, val);
}

Q_INVOKABLE void Mediator::setShaderUniformValue(const char *name, const int &val)
{
	m_scene->setShaderUniformValue(name, val);
}

Q_INVOKABLE void Mediator::setShaderUniformValue(const char *name, const float &x, const float &y, const float &z)
{
	m_scene->setShaderUniformValue(name, x, y, z);
}
