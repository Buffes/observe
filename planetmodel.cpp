#include "planetmodel.h"
#include "calculate_positions.h"
#include <QFile>
#include <QThread>

PlanetModel::PlanetModel(QObject *parent):
    QAbstractListModel(parent)
{
    this->data_manager = DataManager::getInstance();

    scale_factor = 25.0;
    m_workerThread = new WorkerThread(data_manager->m_planets, QDateTime::currentDateTime());
    QObject::connect(m_workerThread, &WorkerThread::new_positions,
                     this, &PlanetModel::updatePositions);
    QObject::connect(this, &PlanetModel::new_date_input,
                     m_workerThread, &WorkerThread::set_date);
}


QHash<int, QByteArray> PlanetModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[XRole] = "x";
    roles[YRole] = "y";
    roles[ZRole] = "z";
    roles[ColorRole] = "p_color";
    return roles;
}

int PlanetModel::rowCount(const QModelIndex& parent) const {
    return data_manager->m_planet_positions.size();
}

QVariant PlanetModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= data_manager->m_planet_positions.size())
        return QVariant();
    QVariant v;
    switch (role) {
        // NOTE: QML seems to only support +Y as up. The math uses +Z. So we convert systems here (swap Y and Z, and negate Z).
        case XRole:
            return data_manager->m_planet_positions[index.row()].x;
        case YRole:
            return data_manager->m_planet_positions[index.row()].z;
        case ZRole:
            return -data_manager->m_planet_positions[index.row()].y;
        case ColorRole:
            v = data_manager->m_planets[index.row()].color;
            return v;
        default:
            return QVariant();
    }
}

void PlanetModel::updatePositions(QList<dVector3D> positions) {
    // scale positions for visualization purposes. units in are AU
    for (dVector3D &pos : positions) {
            pos.x *= scale_factor;
            pos.y *= scale_factor;
            pos.z *= scale_factor;
    }

    if (data_manager->m_planet_positions.size() < positions.size()) {
            beginInsertRows(QModelIndex(), data_manager->m_planet_positions.size(), data_manager->m_planet_positions.size() + positions.size() - 1);
            this->data_manager->m_planet_positions = positions;
            endInsertRows();
    }
    else {
            data_manager->m_planet_positions = positions;
            emit dataChanged(createIndex(0, 0), createIndex(data_manager->m_planet_positions.size()-1, 0));
    }
}

void PlanetModel::calculatePositions(QDateTime datetime) {
    emit new_date_input(datetime);
    if (!m_workerThread->active) {
            QList<dVector3D> positions = calc::calculatePositions(data_manager->m_planets, datetime);
        updatePositions(positions);
    }
}

void PlanetModel::calculatePositionsRepeatedly() {
    if (!m_workerThread->active)
        m_workerThread->start();
    else
        m_workerThread->disable();
}

void PlanetModel::setAnimationSpeed(double value) {
    m_workerThread->set_speed(value);
}
