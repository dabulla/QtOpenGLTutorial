#include <QGuiApplication>

#include "window.h"

#include <QDebug>

int main( int argc, char* argv[] )
{
    QGuiApplication a( argc, argv);
    Window w;
	//w.setSource(QUrl("ui/overlay.qml"));
    w.show();
	//QQuickView  viewer;
    //viewer.setSource(QUrl("ui/overlay.qml"));
    //viewer.show();

    return a.exec();
}
