#include <QGuiApplication>

#include "window.h"

#include <QDebug>

int main( int argc, char* argv[] )
{
    QGuiApplication a( argc, argv);
    Window w;
	//w.setWidth(800);
	//w.setHeight(450);
	//w.setSource(QUrl("ui/shader.qml"));
    w.show();
	//QQuickView  viewer;
    //viewer.setSource(QUrl("ui/overlay.qml"));
    //viewer.show();

    return a.exec();
}
