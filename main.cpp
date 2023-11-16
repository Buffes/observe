#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQuick3D/qquick3d.h>
#include <QImageReader>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QSurfaceFormat::setDefaultFormat(QQuick3D::idealSurfaceFormat());
    //QImageReader::setAllocationLimit(512+64);

    QQmlApplicationEngine engine;
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("observe", "Main");

    return app.exec();
}
