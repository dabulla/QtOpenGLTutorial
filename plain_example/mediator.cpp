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
		m_scene->setRootObject(m_uiWnd->rootObject());
	}
	else
	{
		m_uiWnd->destroy();
		m_uiWnd = 0;
		m_mainWnd->rootObject()->setOpacity(1);
		m_scene->setRootObject(m_mainWnd->rootObject());
	}
}