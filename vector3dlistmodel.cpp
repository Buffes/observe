#include "vector3dlistmodel.h"
#include "calculate_positions.h"
#include <QFile>
#include <QTimer>

Vector3DListModel::Vector3DListModel(QObject *parent) {
    loadBodies("../observe/orbital_elements.txt");
    m_data.reserve(m_bodyCount + 1);
}

void Vector3DListModel::loadBodies(QString path) {
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
             if (line.trimmed().length() == 0) {
                 continue; // skip empty lines
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
             }
         }
         this->m_bodies.push_back(current_body); // last item
     }
     this->m_bodyCount += this->m_bodies.size();
}

QHash<int, QByteArray> Vector3DListModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[XRole] = "x";
    roles[YRole] = "y";
    roles[ZRole] = "z";
    return roles;
}

int Vector3DListModel::rowCount(const QModelIndex& parent) const {
    return m_data.size();
}

QVariant Vector3DListModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= m_data.size())
        return QVariant();
    switch (role) {
        case XRole:
            return m_data[index.row()].x;
        case YRole:
            return m_data[index.row()].y;
        case ZRole:
            return m_data[index.row()].z;
        default:
            return QVariant();
    }
}

void Vector3DListModel::calculatePositions(QDateTime datetime) {
    int year  = datetime.date().year();
    int month = datetime.date().month();
    int day   = datetime.date().day();
    int hours = datetime.time().hour();
    int mins  = datetime.time().minute();
    int secs  = datetime.time().second();

    QList<dVector3D> positions = calc::calculatePositions(this->m_bodies, year, month, day, hours, mins, secs);

    // The units of the positions are given in AU,
    /*double scale_factor = 10.0;
    for (auto &pos : positions) {
            pos.x *= scale_factor;
            pos.y *= scale_factor;
            pos.z *= scale_factor;
    }*/

    if (m_data.size() < positions.size()) {
        beginInsertRows(QModelIndex(), m_data.size(), m_data.size() + positions.size());
        this->m_data = positions;
        endInsertRows();
    }
    else {
        this->m_data = positions;
        emit dataChanged(createIndex(0, 0), createIndex(m_data.size(), 0));
    }
}

void Vector3DListModel::calculatePositionsRepeatedly(QDateTime start_date) {
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=]() {Vector3DListModel::calculatePositions(start_date);});
    timer->start(1000);
}
