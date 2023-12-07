#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQuick3D/qquick3d.h>
#include <QImageReader>

#include "datamanager.h"
#include "planetmodel.h"
#include "starInstanceTable.h"
#include "selectionhandler.h"

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

    PlanetModel planet_model;
    StarInstanceTable star_instance_table;
    SelectionHandler selection_handler;

    QQmlApplicationEngine engine;
    engine.setInitialProperties({
        {"planetModel", QVariant::fromValue(&planet_model)},
        {"starInstanceTable", QVariant::fromValue(&star_instance_table)},
        {"selectionHandler", QVariant::fromValue(&selection_handler)}
    });
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
