#ifndef MEDIATOR_H
#define MEDIATOR_H

#include <QObject>

class ShaderTestScene;
class Mediator : public QObject
{
	Q_OBJECT

public:
	Mediator(QObject *parent, ShaderTestScene *scene);
	~Mediator();

    Q_INVOKABLE void reloadShader();
private:
	ShaderTestScene *m_scene;
};

#endif // MEDIATOR_H
