#include "planetmodel.h"
#include "calculate_positions.h"
#include <QFile>
#include <QThread>

PlanetModel::PlanetModel(QObject *parent) {
    loadBodies("../observe/orbital_elements.txt");
    m_data.reserve(m_bodyCount);
    scale_factor = 25.0;
    m_workerThread = new WorkerThread(m_bodies, QDateTime::currentDateTime());
    QObject::connect(m_workerThread, &WorkerThread::new_positions,
                     this, &PlanetModel::update_positions);
    QObject::connect(this, &PlanetModel::new_date_input,
                     m_workerThread, &WorkerThread::set_date);
}

void PlanetModel::loadBodies(QString path) {
     QFile file(path);
     if (!file.exists()) {
         qWarning() << "Could not find file " << path;
         return;
     }
     if (file.open(QFile::ReadOnly))  {
         QTextStream in(&file);
         CelestialBody current_body;
         while (!in.atEnd()) {
             QString line = in.readLine();
             if (line.trimmed().length() == 0 || line.startsWith("//")) {
                 continue; // skip empty lines and comments
             }
             if (line.startsWith("[")) {
                 if (current_body.name.length() > 0) {
                     this->m_bodies.push_back(current_body);
                 }
                 current_body = {0};
                 current_body.name = line.sliced(1, line.length() - 2);
             }
             else {
                 QStringList parts = line.split(",");
                 QString element = parts[0];
                 double base_value = parts[1].toDouble();
                 double delta = parts[2].toDouble();
                 if (element.startsWith("N")) {
                     current_body.base_elements.N = base_value;
                     current_body.delta.N = delta;
                 }
                 else if (element.startsWith("i")) {
                     current_body.base_elements.i = base_value;
                     current_body.delta.i = delta;
                 }
                 else if (element.startsWith("w")) {
                     current_body.base_elements.w = base_value;
                     current_body.delta.w = delta;
                 }
                 else if (element.startsWith("a")) {
                     current_body.base_elements.a = base_value;
                     current_body.delta.a = delta;
                 }
                 else if (element.startsWith("e")) {
                     current_body.base_elements.e = base_value;
                     current_body.delta.e = delta;
                 }
                 else if (element.startsWith("M")) {
                     current_body.base_elements.M = base_value;
                     current_body.delta.M = delta;
                 }
                 else if (element.startsWith("color")) {
                     current_body.color = QColor(parts[1].toInt(), parts[2].toInt(), parts[3].toInt());
                 }
             }
         }
         this->m_bodies.push_back(current_body); // last item
     }
     this->m_bodyCount += this->m_bodies.size();
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
    return m_data.size();
}

QVariant PlanetModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= m_data.size())
        return QVariant();
    QVariant v;
    switch (role) {
        // NOTE: QML seems to only support +Y as up. The math uses +Z. So we swap them here.
        case XRole:
            return m_data[index.row()].x;
        case YRole:
            return m_data[index.row()].z;
        case ZRole:
            return m_data[index.row()].y;
        case ColorRole:
            v = m_bodies[index.row()].color;
            return v;
        default:
            return QVariant();
    }
}

void PlanetModel::update_positions(QList<dVector3D> positions) {
    // scale positions for visualization purposes. units in are AU
    for (dVector3D &pos : positions) {
            pos.x *= scale_factor;
            pos.y *= scale_factor;
            pos.z *= scale_factor;
    }

    if (m_data.size() < positions.size()) {
            beginInsertRows(QModelIndex(), m_data.size(), m_data.size() + positions.size() - 1);
            this->m_data = positions;
            endInsertRows();
    }
    else {
            this->m_data = positions;
            emit dataChanged(createIndex(0, 0), createIndex(m_data.size()-1, 0));
    }
}

void PlanetModel::calculatePositions(QDateTime datetime) {
    emit new_date_input(datetime);
    if (!m_workerThread->active) {
        QList<dVector3D> positions = calc::calculatePositions(this->m_bodies, datetime);
        update_positions(positions);
    }
}

void PlanetModel::calculatePositionsRepeatedly() {
    if (!m_workerThread->active)
        m_workerThread->start();
    else
        m_workerThread->disable();
}

void PlanetModel::set_animation_speed(double value) {
    m_workerThread->set_speed(value);
}