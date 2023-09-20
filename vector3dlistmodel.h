#ifndef VECTOR3DLISTMODEL_H
#define VECTOR3DLISTMODEL_H

#include <QObject>
#include <QQmlEngine>
#include <QAbstractListModel>
#include "datastructures.h"

class Vector3DListModel : public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT

public:
    // Model related things
    Vector3DListModel(QObject *parent = 0);
    QHash<int, QByteArray> roleNames() const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role) const;

    enum CustomRoles{
        XRole=Qt::UserRole+1,
        YRole,
        ZRole,

    };

    void loadBodies(QString path);

public slots:
    void calculatePositions(QDateTime date);
    void calculatePositionsRepeatedly(QDateTime start_date);
    //void calculatePositions(int year, int month, int day, int hours, int minutes, int seconds);

private:
    QList<dVector3D> m_data;
    QList<CelestialBody> m_bodies;
    int m_bodyCount;
};

#endif // VECTOR3DLISTMODEL_H
