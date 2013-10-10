#ifndef MEDIATOR_H
#define MEDIATOR_H

#include <QObject>
#include <qquickview.h>
#include <qvariant.h>
#include "shadertestscene.h"

class ShaderTestScene;
class Window;

//Note: typedefs must not be used with Q_PROPERTYs: Qml won't recognise "ShaderItem" as a registered qml datatype
//typedef QVariant ShaderItem;

class Mediator : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QVariant selectedShader READ selectedShader WRITE setSelectedShader NOTIFY selectedShaderChanged)

signals:
	void selectedShaderChanged(ShaderInfo shader);
public:
	Mediator(QObject *parent, ShaderTestScene *scene, Window *w);
	~Mediator();

    Q_INVOKABLE void reloadShader();
    Q_INVOKABLE void toggleDialog();

    Q_INVOKABLE void enableRotation();
    Q_INVOKABLE void disableRotation();

    Q_INVOKABLE void setCamerModeWalkthrough();
    Q_INVOKABLE void setCamerModeObjectInspection();

    Q_INVOKABLE void setObjectBunny();
    Q_INVOKABLE void setObjectPlane();

    Q_INVOKABLE void setCullmodeBack();
    Q_INVOKABLE void setCullmodeFront();
    Q_INVOKABLE void setCullmodeBoth();
    Q_INVOKABLE void setCullmodeNone();

	// For qml the methods must have different names. Otherwise qml can not decide which method to use.
	Q_INVOKABLE void setShaderUniformValue1f(const QString &name, const float &val);
	Q_INVOKABLE void setShaderUniformValue1i(const QString &name, const int &val);
	Q_INVOKABLE void setShaderUniformValue3f(const QString &name, const float &x, const float &y, const float &z);
	Q_INVOKABLE void setShaderUniformValue1b(const QString &name, const bool &val);


    void setSelectedShader(const QVariant &shader)
    {
        m_selectedShader = shader;
		ShaderInfo shaderInfo;
		shaderInfo.vertexShaderFile = shader.value<QObject*>()->property("vertexShaderFile").toString();
		shaderInfo.vertexShaderProc = shader.value<QObject*>()->property("vertexShaderProc").toString();
		shaderInfo.tesselationControlShaderFile = shader.value<QObject*>()->property("tesselationControlShaderFile").toString();
		shaderInfo.tesselationControlShaderProc = shader.value<QObject*>()->property("tesselationControlShaderProc").toString();
		shaderInfo.tesselationEvaluationShaderFile = shader.value<QObject*>()->property("tesselationEvaluationShaderFile").toString();
		shaderInfo.tesselationEvaluationShaderProc = shader.value<QObject*>()->property("tesselationEvaluationShaderProc").toString();
		shaderInfo.geometryShaderFile = shader.value<QObject*>()->property("geometryShaderFile").toString();
		shaderInfo.geometryShaderProc = shader.value<QObject*>()->property("geometryShaderProc").toString();
		shaderInfo.fragmentShaderFile = shader.value<QObject*>()->property("fragmentShaderFile").toString();
		shaderInfo.fragmentShaderProc = shader.value<QObject*>()->property("fragmentShaderProc").toString();
        emit selectedShaderChanged(shaderInfo);
    }

    QVariant selectedShader() const
    { return m_selectedShader; }

private:
	Window *m_mainWnd;
	ShaderTestScene *m_scene;
	QQuickView *m_uiWnd;

	QVariant m_selectedShader;
 public:
};

Q_DECLARE_METATYPE(Mediator*)

#endif // MEDIATOR_H
