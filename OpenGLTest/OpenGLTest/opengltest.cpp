#include "opengltest.h"

#include <QTimer>

OpenGLTest::OpenGLTest(QScreen *screen)
	: QWindow(screen), m_indexBuffer(QOpenGLBuffer::IndexBuffer), m_logger(this)
{
	    connect( &m_logger, SIGNAL( messageLogged( QOpenGLDebugMessage ) ),
             this, SLOT( onMessageLogged( QOpenGLDebugMessage ) ),
             Qt::DirectConnection );
 
    if ( m_logger.initialize() ) {
        m_logger.startLogging( QOpenGLDebugLogger::SynchronousLogging );
        m_logger.enableMessages();
    }

    // Tell Qt we will use OpenGL for this window
    setSurfaceType( OpenGLSurface );
 
    // Specify the format and create platform-specific surface
    QSurfaceFormat format;
    format.setDepthBufferSize( 24 );
    format.setMajorVersion( 4 );
    format.setMinorVersion( 3 );
    format.setSamples( 4 );
    format.setProfile( QSurfaceFormat::CoreProfile );
    format.setOption( QSurfaceFormat::DebugContext );
	setFormat( format );
	create();
    // Create an OpenGL context
    m_context = new QOpenGLContext;
    m_context->setFormat( format );
    m_context->create();
 
    // Make the context current on this window
    m_context->makeCurrent( this );
 
    // Obtain a functions object and resolve all entry points
    // m_funcs is declared as: QOpenGLFunctions_4_3_Core* m_funcs
    m_funcs = m_context->versionFunctions<QOpenGLFunctions_4_0_Core>();
    if ( !m_funcs ) {
        qWarning( "Could not obtain OpenGL versions object" );
        exit( 1 );
	}
	m_funcs->initializeOpenGLFunctions();
	initialize();
}

void OpenGLTest::onMessageLogged( QOpenGLDebugMessage message )
{
    qDebug() << message;
}

OpenGLTest::~OpenGLTest()
{

}
static unsigned int octa_indices[8][3]=
{
    {0,1,2},{0,2,3},
    {0,3,4},{0,4,1},
    {5,2,1},{5,3,2},
    {5,4,3},{5,1,4}
};
 
static float octa_verts[6][3]=
{    
    {0,0,-1},{1,0,0},
    {0,-1,0},{-1,0,0},
    {0,1,0},{0,0,1}
};

void OpenGLTest::initialize()
{
    // Assumes we have a current QOpenGLContext and that
    // m_shaderProgram is a QOpenGLShaderProgram
 
	bool bVert = m_shaderProgram.addShaderFromSourceFile( QOpenGLShader::Vertex, QStringLiteral("../shaders/simple.vert"));
	bool bFrag = m_shaderProgram.addShaderFromSourceFile( QOpenGLShader::Fragment, QStringLiteral("../shaders/simple.frag"));

    // Enable depth testing
    glEnable( GL_DEPTH_TEST );
    //glEnable( GL_CULL_FACE );

    glClearColor( 0.65f, 0.77f, 1.0f, 1.0f );

    // Create VAO for first object to render
    m_vao1 = new QOpenGLVertexArrayObject( this );
    m_vao1->create();
    m_vao1->bind();
 
    // Setup VBOs and IBO (use QOpenGLBuffer to buffer data,
    // specify format, usage hint etc). These will be
    // remembered by the currently bound VAO
    m_positionBuffer.create();
    m_positionBuffer.setUsagePattern( QOpenGLBuffer::StreamDraw );
    m_positionBuffer.bind();
    m_positionBuffer.allocate( octa_verts, 6 * 3 * sizeof( float ) );
    m_shaderProgram.enableAttributeArray( "vertexPosition" );
    m_shaderProgram.setAttributeBuffer( "vertexPosition", GL_FLOAT, 0, 3 );

    m_indexBuffer.create();
	//m_indexBuffer.setUsagePattern( );
    m_indexBuffer.bind();
    m_indexBuffer.allocate( octa_indices, 8 * 3 * sizeof( int ) );
 
    //m_colorBuffer.create();
    //m_colorBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    //m_colorBuffer.bind();
    //m_colorBuffer.allocate( colorData,
    //                        vertexCount * 3 * sizeof( float ) );
    //m_shaderProgram.enableAttributeArray( "vertexColor" );
    //m_shaderProgram.setAttributeBuffer( "vertexColor", GL_FLOAT, 0, 3 );
 
    // Repeat for buffers of normals, texture coordinates,
    // tangents, ...
    //...
 
    // Create VAO for second object to render
    //m_vao2 = new QOpenGLVertexArrayObject( this );
    //m_vao2->create();
    //m_vao2->bind();
 
    // Setup VBOs and IBO for next object
    //...
 
    // Rinse and repeat for other objects
    //m_skyBoxVAO = new QOpenGLVertexArrayObject( this );
    //...
	
    QTimer* timer = new QTimer( this );
    connect( timer, SIGNAL( timeout() ), this, SLOT( render() ) );
    timer->start( 16 );
}
 
void OpenGLTest::render()
{
    // Make the context current
    m_context->makeCurrent( this );

    // Clear buffers
    m_funcs->glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
 
    // Bind shader program, textures for first set of objects
    m_shaderProgram.bind();
    // Switch to the vertex data for first object and draw it
    m_vao1->bind();
    //m_funcs->glDrawElements(GL_TRIANGLES, 8, GL_UNSIGNED_INT, );
	m_funcs->glDrawArrays(GL_TRIANGLES, 0, 8);
    // Switch to the vertex data for second object and draw it
    //m_vao2->bind();
    //m_funcs->glDrawElements(...);
 
    // Maybe change shader program, textures etc
    // and draw other objects
    //m_skyboxShaderProgram->bind();
    //...
    //m_skyboxVAO->bind();
    //m_funcs->glDrawElements(...);
    //...
    // Swap front/back buffers
    m_context->swapBuffers( this );
}