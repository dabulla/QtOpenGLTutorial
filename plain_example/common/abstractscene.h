#ifndef ABSTRACTSCENE_H
#define ABSTRACTSCENE_H

#include <QObject>

class QOpenGLContext;
class QQuickItem;

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

	virtual void setRootObject(QQuickItem* ctx) = 0;
protected:
    QOpenGLContext* m_context;
};

#endif // ABSTRACTSCENE_H
