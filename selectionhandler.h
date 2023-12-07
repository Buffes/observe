#ifndef SELECTIONHANDLER_H
#define SELECTIONHANDLER_H

#include <QObject>
#include <QtQmlIntegration>
#include <QVector3D>
#include "datamanager.h"

class SelectionHandler : public QObject {
    Q_OBJECT
    QML_ELEMENT
public:
    SelectionHandler();
    Q_INVOKABLE void rayPick(QVector3D origin, QVector3D direction);

    DataManager *data_manager;
};

#endif // SELECTIONHANDLER_H
