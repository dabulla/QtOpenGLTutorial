#ifndef WINDOW_H
#define WINDOW_H

#include <qquickview.h>
#include <QTime>

class AbstractScene;

class QOpenGLContext;

//#define WINDOW_BASE QWindow
#define WINDOW_BASE QQuickView

class Window : public WINDOW_BASE
{
    Q_OBJECT

public:
    Window( QWindow* window = 0 );

	QUrl m_sourcePath;
	void loadUi();
private:
    void initializeGL();

protected slots:
    void resizeGL();
    void updateAndRenderScene();

protected:
    void keyPressEvent( QKeyEvent* e );
    void keyReleaseEvent( QKeyEvent* e );
    void mousePressEvent( QMouseEvent* e );
    void mouseReleaseEvent( QMouseEvent* e );
    void mouseMoveEvent( QMouseEvent* e );
	QSurfaceFormat getFormat();
	
	void closing(QQuickCloseEvent *close);
protected slots :
    void onSceneGraphInitialized();

private:
    QOpenGLContext* m_context;
    AbstractScene* m_scene;
    bool m_leftButtonPressed;
    QPoint m_prevPos;
    QPoint m_pos;
    QTime m_time;
};

#endif // WINDOW_H
