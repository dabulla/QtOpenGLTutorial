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
	//m_mainWnd->rootObject()->setVisible(false);
	if(	m_uiWnd == 0)
	{
		m_uiWnd = new QQuickView();//m_mainWnd);
		m_uiWnd->setSource(m_mainWnd->m_sourcePath);
		m_uiWnd->setResizeMode(QQuickView::SizeRootObjectToView);
		m_uiWnd->setWidth(200);
		m_uiWnd->setHeight(500);
		m_uiWnd->setSurfaceType( QSurface::OpenGLSurface );
		m_uiWnd->rootContext()->setContextProperty("application", this);
		//m_uiWnd->setFlags(Qt::Popup | Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
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

Q_INVOKABLE void Mediator::setShaderUniformValue1f(const QString &name, const float &val)
{
	m_scene->setShaderUniformValue(name.toStdString().c_str(), val);
}

Q_INVOKABLE void Mediator::setShaderUniformValue1i(const QString &name, const int &val)
{
	m_scene->setShaderUniformValue(name.toStdString().c_str(), val);
}

Q_INVOKABLE void Mediator::setShaderUniformValue3f(const QString &name, const float &x, const float &y, const float &z)
{
	m_scene->setShaderUniformValue(name.toStdString().c_str(), x, y, z);
}

Q_INVOKABLE void Mediator::setShaderUniformValue1b(const QString &name, const bool &val)
{
	m_scene->setShaderUniformValue(name.toStdString().c_str(), val);
}

Q_INVOKABLE void Mediator::setCamerModeWalkthrough()
{
	m_scene->setCamerModeWalkthrough();
}

Q_INVOKABLE void Mediator::setCamerModeObjectInspection()
{
	m_scene->setCamerModeObjectInspection();
}
Q_INVOKABLE void Mediator::setCullmodeBack()
{
	m_scene->setGlCullMode(GL_BACK);
}

Q_INVOKABLE void Mediator::setCullmodeFront()
{
	m_scene->setGlCullMode(GL_FRONT);
}

Q_INVOKABLE void Mediator::setCullmodeBoth()
{
	m_scene->setGlCullMode(GL_FRONT_AND_BACK);
}

Q_INVOKABLE void Mediator::setCullmodeNone()
{
	m_scene->setGlCullMode(GL_NONE);
}

Q_INVOKABLE void Mediator::enableRotation()
{
	m_scene->setRotationSpeed(10.1f);
}

Q_INVOKABLE void Mediator::disableRotation()
{
	m_scene->setRotationSpeed(0.0f);
}