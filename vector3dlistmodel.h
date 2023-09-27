#ifndef VECTOR3DLISTMODEL_H
#define VECTOR3DLISTMODEL_H

#include <QObject>
#include <QQmlEngine>
#include <QAbstractListModel>
#include <QThread>
#include "datastructures.h"
#include "calculate_positions.h"

class WorkerThread : public QThread {
    Q_OBJECT

    void run() override {
        active = true;
        while (active) {
            //qDebug() << "hello";
            QList<dVector3D> positions = calc::calculatePositions(bodies, date);
            date = date.addDays(2);
            emit new_positions(positions);
            this->msleep(17);
        }
    }

public:
    WorkerThread(QList<CelestialBody> bodies, QDateTime start_date, QObject *parent = 0)
    : QThread(parent) {
        this->bodies = bodies;
        this->date = start_date;
    }

    void disable() {
        active = false;
    }

    QList<CelestialBody> bodies;
    QDateTime date;
    bool active;

signals:
    void new_positions(QList<dVector3D> positions);

public slots:
    void set_date(QDateTime datetime) {
        this->date = datetime;
    }
};


enum Visualization {
    Heliocentric,
    Geocentric,
    StarChart,
};


class Vector3DListModel : public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT

public:
    // Model related things
    Vector3DListModel(QObject *parent = 0);
    //  ~Vector3DListModel();
    QHash<int, QByteArray> roleNames() const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role) const;

    enum CustomRoles{
        XRole=Qt::UserRole+1,
        YRole,
        ZRole,
        ColorRole,
    };

    void loadBodies(QString path);

public slots:
    void calculatePositions(QDateTime date);
    void calculatePositionsRepeatedly();
    void update_positions(QList<dVector3D> positions);
    //void calculatePositions(int year, int month, int day, int hours, int minutes, int seconds);

signals:
    void new_date_input(QDateTime datetime);

private:
    // data for each planet.
    QList<dVector3D> m_data;
    QList<CelestialBody> m_bodies;
    WorkerThread *m_workerThread;
    int m_bodyCount;
    double scale_factor;
    Visualization visualization;
};

#endif // VECTOR3DLISTMODEL_H
