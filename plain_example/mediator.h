#ifndef MEDIATOR_H
#define MEDIATOR_H

#include <QObject>
#include <qquickview.h>

class ShaderTestScene;
class Window;

class Mediator : public QObject
{
	Q_OBJECT

public:
	Mediator(QObject *parent, ShaderTestScene *scene, Window *w);
	~Mediator();

    Q_INVOKABLE void reloadShader();
    Q_INVOKABLE void toggleDialog();
private:
	Window *m_mainWnd;
	ShaderTestScene *m_scene;
	QQuickView *m_uiWnd;
};

#endif // MEDIATOR_H
