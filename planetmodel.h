#ifndef PLANETMODEL_H
#define PLANETMODEL_H

#include <QObject>
#include <QQmlEngine>
#include <QAbstractListModel>
#include <QThread>
#include <QDeadlineTimer>
#include <QElapsedTimer>
#include <time.h>
#include "datastructures.h"
#include "calculate_positions.h"
#include "datamanager.h"


class WorkerThread : public QThread {
    Q_OBJECT

    void run() override {
        active = true;

        while (active) {
            QDeadlineTimer deadline(16); // 60 fps is about 16.67 ms
            QList<dVector3D> positions = calc::calculatePositions(bodies, date);
            date = date.addSecs(secs_per_update);
            emit new_positions(positions);

            qint64 time_left = deadline.remainingTime();
            if (time_left > 0) {
                this->msleep(time_left - 1);

                while (!deadline.hasExpired()) {
                    // spin last millisecond to avoid overshooting.
                }
            }
        }
    }

public:
    WorkerThread(QList<CelestialBody> bodies, QDateTime start_date, QObject *parent = 0)
    : QThread(parent) {
        this->bodies = bodies;
        this->date = start_date;
        this->secs_per_update = 3600 * 24;
    }

    void disable() {
        active = false;
    }

    void set_speed(double speed) {
        this->secs_per_update = (int64_t)(3600.0 * 24.0 * speed);
    }

    QList<CelestialBody> bodies;
    QDateTime date;
    bool active;
    int64_t secs_per_update;

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

class PlanetModel : public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT

public:
    // Model related things
    PlanetModel(QObject *parent = 0);
    //  ~Vector3DListModel();
    QHash<int, QByteArray> roleNames() const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role) const;

    enum CustomRoles {
        XRole=Qt::UserRole+1,
        YRole,
        ZRole,
        ColorRole,
        RadiusRole,
    };

public slots:
    void calculatePositions(QDateTime date);
    void calculatePositionsRepeatedly();
    void updatePositions(QList<dVector3D> positions);
    void setAnimationSpeed(double value);
    //void calculatePositions(int year, int month, int day, int hours, int minutes, int seconds);

signals:
    void new_date_input(QDateTime datetime);

private:
    DataManager *data_manager;
    WorkerThread *m_workerThread;
    double distance_from_center;
    Visualization visualization;
};

#endif // PLANETMODEL_H
