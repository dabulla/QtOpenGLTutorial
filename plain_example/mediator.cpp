#include "mediator.h"
#include "shadertestscene.h"

Mediator::Mediator(QObject *parent, ShaderTestScene *scene)
	: QObject(parent),
	m_scene(scene)
{
	
}

Mediator::~Mediator()
{
}

Q_INVOKABLE void Mediator::reloadShader()
{
	m_scene->prepareShaders();
}