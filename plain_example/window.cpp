#include "window.h"

#include "terraintessellationscene.h"

#include <QCoreApplication>
#include <QKeyEvent>
#include <QOpenGLContext>
#include <QTimer>

Window::Window( QWindow* window )
    : QQuickView( window ),
      m_scene( new TerrainTessellationScene( this ) ),
      m_leftButtonPressed( false )
{
	setClearBeforeRendering(false);
    //show();
    // Tell Qt we will use OpenGL for this window
    setSurfaceType( OpenGLSurface );
	setPersistentOpenGLContext(true);

    // Specify the format we wish to use
    QSurfaceFormat format;
    format.setDepthBufferSize( 24 );
    format.setMajorVersion( 4 );
    format.setMinorVersion( 3 );
    format.setSamples( 4 );
    format.setProfile( QSurfaceFormat::CoreProfile );
    format.setOption( QSurfaceFormat::DebugContext );

    resize( 1366, 768 );
    //setFormat( format );
    //create();

    // Create an OpenGL context
    //m_context->setFormat( format );
    //m_context->create();

    // Setup our scene
    //m_context->makeCurrent( this );

    //initializeGL();
	//m_context->doneCurrent();

    // Make sure we tell OpenGL about new window sizes

    // This timer drives the scene updates
    //QTimer* timer = new QTimer( this );
	connect( this, SIGNAL( beforeRendering() ), this, SLOT( updateScene() ), Qt::DirectConnection );
    //timer->start( 16 );
	//connect(this, SIGNAL(sceneGraphInitialized()), this, SLOT(onSceneGraphInitialized()), Qt::DirectConnection );
}

void Window::initializeGL()
{
    m_context->makeCurrent( this );
    m_scene->initialise();
    m_time.start();
	m_context->doneCurrent();
}

void Window::paintGL()
{
    // Make the context current
    m_context->makeCurrent( this );

    // Do the rendering (to the back buffer)
    m_scene->render();

    // Swap front/back buffers
    m_context->swapBuffers( this );
	m_context->doneCurrent();
}

void Window::resizeGL()
{
    m_context->makeCurrent( this );
    m_scene->resize( width(), height() );
	m_context->doneCurrent();
}

void Window::updateScene()
{
	static bool firstTime = true;
	if(firstTime)
	{
		m_context = openglContext();//new QOpenGLContext;
		m_scene->setContext(m_context);
		initializeGL();
		resizeGL();
		
		connect( this, SIGNAL( widthChanged( int ) ), this, SLOT( resizeGL() ) );
		connect( this, SIGNAL( heightChanged( int ) ), this, SLOT( resizeGL() ) );
		firstTime = false;
	}
    float time = m_time.elapsed() / 1000.0f;
    m_scene->update( time );
    paintGL();
}

void Window::onSceneGraphInitialized()
{
	//openglContext()->blockSignals(true);
	//openglContext()->doneCurrent();
	//openglContext()->setShareContext(m_context);
	//openglContext()->setFormat(requestedFormat());
	//openglContext()->create();
	//openglContext()->makeCurrent(this);
	//openglContext()->blockSignals(false);
}

void Window::keyPressEvent( QKeyEvent* e )
{
    const float speed = 44.7f; // in m/s. Equivalent to 100 miles/hour
    TerrainTessellationScene* scene = static_cast<TerrainTessellationScene*>( m_scene );
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

        case Qt::Key_Shift:
            scene->setViewCenterFixed( true );
            break;

        case Qt::Key_Plus:
            scene->setTerrainHorizontalScale( scene->terrainHorizontalScale() + 0.1 );
            break;

        case Qt::Key_Minus:
            scene->setTerrainHorizontalScale( scene->terrainHorizontalScale() - 0.1 );
            break;

        case Qt::Key_Home:
            scene->setTerrainVerticalScale( scene->terrainVerticalScale() + 0.05 );
            break;

        case Qt::Key_End:
            scene->setTerrainVerticalScale( scene->terrainVerticalScale() - 0.05 );
            break;

        case Qt::Key_BracketLeft:
            scene->setSunAngle( scene->sunAngle() - 0.2 );
            break;

        case Qt::Key_BracketRight:
            scene->setSunAngle( scene->sunAngle() + 0.2 );
            break;

        case Qt::Key_Comma:
            scene->setScreenSpaceError( scene->screenSpaceError() + 0.1 );
            break;

        case Qt::Key_Period:
            scene->setScreenSpaceError( scene->screenSpaceError() - 0.1 );
            break;

        case Qt::Key_F1:
            scene->setDisplayMode( TerrainTessellationScene::TexturedAndLit );
            break;

        case Qt::Key_F2:
            scene->setDisplayMode( TerrainTessellationScene::SimpleWireFrame );
            break;

        case Qt::Key_F3:
            scene->setDisplayMode( TerrainTessellationScene::WorldHeight );
            break;

        case Qt::Key_F4:
            scene->setDisplayMode( TerrainTessellationScene::WorldNormals );
            break;

        case Qt::Key_F5:
            scene->setDisplayMode( TerrainTessellationScene::Grass );
            break;

        case Qt::Key_F6:
            scene->setDisplayMode( TerrainTessellationScene::GrassAndRocks );
            break;

        case Qt::Key_F7:
            scene->setDisplayMode( TerrainTessellationScene::GrassRocksAndSnow );
            break;

        case Qt::Key_F8:
            scene->setDisplayMode( TerrainTessellationScene::LightingFactors );
            break;

        default:
            QQuickView::keyPressEvent( e );
    }
}

void Window::keyReleaseEvent( QKeyEvent* e )
{
    TerrainTessellationScene* scene = static_cast<TerrainTessellationScene*>( m_scene );
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
            QQuickView::keyReleaseEvent( e );
    }
}

void Window::mousePressEvent( QMouseEvent* e )
{
    if ( e->button() == Qt::LeftButton )
    {
        m_leftButtonPressed = true;
        m_pos = m_prevPos = e->pos();
    }
	QQuickView::mousePressEvent( e );
}

void Window::mouseReleaseEvent( QMouseEvent* e )
{
    if ( e->button() == Qt::LeftButton )
        m_leftButtonPressed = false;
    QQuickView::mouseReleaseEvent( e );
}

void Window::mouseMoveEvent( QMouseEvent* e )
{
    if ( m_leftButtonPressed )
    {
        m_pos = e->pos();
        float dx = 0.2f * ( m_pos.x() - m_prevPos.x() );
        float dy = -0.2f * ( m_pos.y() - m_prevPos.y() );
        m_prevPos = m_pos;

        TerrainTessellationScene* scene = static_cast<TerrainTessellationScene*>( m_scene );
        scene->pan( dx );
        scene->tilt( dy );
    }

    QQuickView::mouseMoveEvent( e );

}
