#include "window.h"

#include "shadertestscene.h"
#include "mediator.h"

#include <QCoreApplication>
#include <QKeyEvent>
#include <QOpenGLContext>
#include <QTimer>
#include <qqmlcontext.h>

Window::Window( QWindow* window )
    : WINDOW_BASE( window ),
      m_scene( new ShaderTestScene( this ) ),
      m_leftButtonPressed( false ),
	  m_sourcePath(QUrl("ui/overlay.qml"))
{
	Mediator* mediator = new Mediator((QObject *)0, static_cast<ShaderTestScene*>(m_scene), this);

	connect( mediator, SIGNAL( selectedShaderChanged(ShaderInfo) ), m_scene, SLOT( setActiveShader(ShaderInfo) ), Qt::DirectConnection );

	rootContext()->setContextProperty("application", mediator);
	loadUi();
	setTitle("ShaderTestScene");
	//Make the UI adopt to the Window on resize
	setResizeMode(QQuickView::SizeRootObjectToView);
    // Tell Qt we will use OpenGL for this window
    setSurfaceType( OpenGLSurface );
	//Qt should not clear the screen before rendering (this clear our renderings)
	setClearBeforeRendering(false);
	//root object should be transparent. This affects the glClearColor (verification needed)
	setColor(QColor(Qt::transparent));

	//setPersistentOpenGLContext(true);
	
    resize( 1366, 768 );

    setFormat( getFormat() );

	

    create();

    // This timer drives the scene updates
    QTimer* timer = new QTimer( this );
	connect( timer, SIGNAL( timeout() ), this, SLOT( update() ), Qt::DirectConnection );
    timer->start( 16 );

	//Use "beforeRendering" to render a scene under the ui.
	connect( this, SIGNAL( beforeRendering() ), this, SLOT( updateAndRenderScene() ), Qt::DirectConnection );

	//Initialize all stuff that need OpenGL+QtQuickUI.
	connect(this, SIGNAL(sceneGraphInitialized()), this, SLOT(onSceneGraphInitialized()), Qt::DirectConnection );

	//connect(this, SIGNAL(this->closing()), this, SLOT(onClose()));
}
void Window::closing(QQuickCloseEvent* close)
{
	exit(0);
	WINDOW_BASE::closing(close);
}

void Window::loadUi()
{
	setSource(m_sourcePath);
}

QSurfaceFormat Window::getFormat()
{
    QSurfaceFormat format;
    format.setVersion( 4, 2 ); //Set OpenGL 4.2
	//Enable AntiAliasing
    format.setSamples( 4 );
	//Make sure there is a depth and alpha buffer
    format.setDepthBufferSize( 24 );
	format.setAlphaBufferSize(8);
	// CompatibilityProfile is used so QtQuick works together with OpenGL 4.2
	// OpenGL 4.2 is used to use nvidia nsight in its current version (only 4.2 supported)
	// Qt makes certain OpenGL-Calls that nsight does not like. Even if the UI is completely disabled, nsight does not work yet.
	format.setProfile( QSurfaceFormat::CompatibilityProfile );
	
	if(DEBUG_OPENGL_ENABLED)
		format.setOption( QSurfaceFormat::DebugContext );

	return format;
}

void Window::onSceneGraphInitialized()
{
		// Create an OpenGL context
		//m_context = new QOpenGLContext();
		//m_context->setFormat( getFormat() );
		//m_context->create();
		//m_context->makeCurrent( this );

		//Borrow Qt's OpenGL-Context
		m_context = openglContext();
		m_scene->setContext(m_context);
		initializeGL();
		// Make sure we tell OpenGL about new window sizes
		resizeGL();
		
		connect( this, SIGNAL( widthChanged( int ) ), this, SLOT( resizeGL() ) );
		connect( this, SIGNAL( heightChanged( int ) ), this, SLOT( resizeGL() ) );

		//The Scene gets a UI-Object so it can look up shader variables
}

void Window::initializeGL()
{
    m_context->makeCurrent( this );
    m_scene->initialise();
    m_time.start();
	//m_context->doneCurrent();
}

void Window::resizeGL()
{
    m_context->makeCurrent( this );
    m_scene->resize( width(), height() );
	//m_context->doneCurrent();
}

void Window::updateAndRenderScene()
{
    float time = m_time.elapsed() / 1000.0f;
    m_scene->update( time );
    m_context->makeCurrent( this );
    // Do the rendering (to the back buffer)
    m_scene->render();

    // Swapping front/back buffers does Qt for us
    //m_context->swapBuffers( this );
	//m_context->doneCurrent();
}

void Window::keyPressEvent( QKeyEvent* e )
{
    const float speed = 44.7f; // in m/s. Equivalent to 100 miles/hour
    ShaderTestScene* scene = static_cast<ShaderTestScene*>( m_scene );
    switch ( e->key() )
    {
        case Qt::Key_Escape:
            QCoreApplication::instance()->quit();
            break;

        case Qt::Key_D:
            scene->setSideSpeed( speed );
            break;

        case Qt::Key_A:
            scene->setSideSpeed( -speed );
            break;

        case Qt::Key_W:
            scene->setForwardSpeed( speed );
            break;

        case Qt::Key_S:
            scene->setForwardSpeed( -speed );
            break;

        case Qt::Key_PageUp:
            scene->setVerticalSpeed( speed );
            break;

        case Qt::Key_PageDown:
            scene->setVerticalSpeed( -speed );
            break;

        default:
            WINDOW_BASE::keyPressEvent( e );
    }
}

void Window::keyReleaseEvent( QKeyEvent* e )
{
    ShaderTestScene* scene = static_cast<ShaderTestScene*>( m_scene );
    switch ( e->key() )
    {
        case Qt::Key_D:
        case Qt::Key_A:
            scene->setSideSpeed( 0.0f );
            break;

        case Qt::Key_W:
        case Qt::Key_S:
            scene->setForwardSpeed( 0.0f );
            break;

        case Qt::Key_PageUp:
        case Qt::Key_PageDown:
            scene->setVerticalSpeed( 0.0f );
            break;

        case Qt::Key_Shift:
            scene->setViewCenterFixed( false );
            break;

        default:
            WINDOW_BASE::keyReleaseEvent( e );
    }
}

void Window::mousePressEvent( QMouseEvent* e )
{
	WINDOW_BASE::mousePressEvent( e );
	if ( !e->isAccepted() && e->button() == Qt::LeftButton )
    {
        m_leftButtonPressed = true;
        m_pos = m_prevPos = e->pos();
    }
}

void Window::mouseReleaseEvent( QMouseEvent* e )
{
    WINDOW_BASE::mouseReleaseEvent( e );
    if (!e->isAccepted() &&  e->button() == Qt::LeftButton )
        m_leftButtonPressed = false;
}

void Window::mouseMoveEvent( QMouseEvent* e )
{
    WINDOW_BASE::mouseMoveEvent( e );
    if (!e->isAccepted() &&  m_leftButtonPressed )
    {
        m_pos = e->pos();
        float dx = 0.2f * ( m_pos.x() - m_prevPos.x() );
        float dy = 0.2f * ( m_pos.y() - m_prevPos.y() );
        m_prevPos = m_pos;

        ShaderTestScene* scene = static_cast<ShaderTestScene*>( m_scene );
        scene->pan( dx );
        scene->tilt( dy );
    }
}
