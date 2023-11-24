#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQuick3D/qquick3d.h>
#include <QImageReader>

#ifdef Q_OS_WIN32
#ifndef NOMINMAX
    #define NOMINMAX
#endif
#include <Windows.h> // for timeBeginPeriod
#endif

int main(int argc, char *argv[])
{
#ifdef Q_OS_WIN32
    timeBeginPeriod(1); // Increase timer resolution on Windows. Qt does not provide this.
#endif

    QGuiApplication app(argc, argv);

    QSurfaceFormat::setDefaultFormat(QQuick3D::idealSurfaceFormat());
    //QImageReader::setAllocationLimit(512+64);

    QQmlApplicationEngine engine;
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("observe", "Main");

    int exec_result = app.exec();

#ifdef Q_OS_WIN32
    timeEndPeriod(1); // Reduce timer resolution Windows.
#endif

    return exec_result;
}
