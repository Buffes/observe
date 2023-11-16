#include "starInstanceTable.h"
#include "calculate_positions.h"
#include <QFile>

StarInstanceTable::StarInstanceTable() {
    loadStarCatalog("../observe/BSC5");
    m_dirty = true;
}

template <typename T>
T readType(QByteArray buffer, size_t *index) {
    T value = *(T*)&buffer[*index];
    *index += sizeof(T);
    return value;
}

void StarInstanceTable::loadStarCatalog(QString path) {
    QFile file(path);
    if (!file.exists()) {
        qWarning() << "Could not find file " << path;
        return;
    }

    if (file.open(QIODevice::ReadOnly)) {
        QByteArray bytes = file.readAll();

        // Parse the header first
        size_t idx = 0;
        int32_t star0          = readType<int32_t>(bytes, &idx); // Subtract from star number to get sequence number
        int32_t first_star     = readType<int32_t>(bytes, &idx); // First star number in file
        int32_t num_stars      = readType<int32_t>(bytes, &idx); // Number of stars in the file. If negative, J2000 is used. If positive, J1950.
        int32_t id_number_type = readType<int32_t>(bytes, &idx);
        int32_t proper_motion  = readType<int32_t>(bytes, &idx);
        int32_t num_magnitudes = readType<int32_t>(bytes, &idx);
        int32_t bytes_per_star = readType<int32_t>(bytes, &idx);

        bool J2000 = num_stars < 0;
        num_stars = abs(num_stars);
        m_stars.reserve(num_stars);
        m_cartesian_coords.reserve(num_stars);

        // TODO: fix the parsing to handle all these cases if we want to laod different star catalogs.
        if (!J2000)              qWarning() << "The star catalog coordinates are in J1950 and not J2000";
        if (!proper_motion)      qWarning() << "The star catalog does not contain proper motion data but we expect it to.";
        if (num_magnitudes != 1) qWarning() << "The star catalog has " << num_magnitudes << " magnitudes per star but we expect only 1";
        if (id_number_type != 1) qWarning() << "The star catalog id number format is " << id_number_type << ", not the expected one (1). Parsing this format is not implemented.";

        // Parse each entry
        for (int i = 0; i < num_stars; i++) {
            StarEntry entry = {};
            entry.id                 = readType<float>(bytes, &idx);
            entry.right_ascension    = readType<double>(bytes, &idx);
            entry.declination        = readType<double>(bytes, &idx);
            entry.spectral_type[0]   = readType<char>(bytes, &idx);
            entry.spectral_type[1]   = readType<char>(bytes, &idx);
            entry.magnitude          = readType<int16_t>(bytes, &idx);
            entry.proper_motion_decl = readType<float>(bytes, &idx);
            entry.proper_motion_ra   = readType<float>(bytes, &idx);

            entry.scale = calc::magnitudeToScale(entry.magnitude);

            m_stars.append(entry);
            m_cartesian_coords.append(calc::RADeclinationToCartesian(entry.right_ascension, entry.declination, 200.0));

        }
    }
}
/*
QHash<int, QByteArray> StarModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[XRole] = "x";
    roles[YRole] = "y";
    roles[ZRole] = "z";
    return roles;
}

int StarModel::rowCount(const QModelIndex& parent) const {
    return m_cartesian_coords.size();
}

QVariant StarModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= m_cartesian_coords.size())
        return QVariant();
    QVariant v;
    switch (role) {
    // NOTE: QML seems to only support +Y as up. The math uses +Z. So we swap them here.
    case XRole:
        return m_cartesian_coords[index.row()].x;
    case YRole:
        return m_cartesian_coords[index.row()].z;
    case ZRole:
        return m_cartesian_coords[index.row()].y;
    default:
        return QVariant();
    }
}
*/
QByteArray StarInstanceTable::getInstanceBuffer(int *instanceCount) {
    if (m_dirty) {
        for (int i = 0; i < m_stars.length(); i++) {
            dVector3D pos = m_cartesian_coords[i];
            float scale = m_stars[i].scale;

            auto entry = calculateTableEntry(
                {(float)pos.x, (float)pos.y, (float)pos.z},
                {1, 1, 1},
                {1, 1, 1},
                QColor(255, 255, 255),
                {(float)pos.x, (float)pos.y, (float)pos.z, scale}
            );
            m_instanceData.append((char*)&entry, sizeof(entry));
        }
        m_instanceCount = m_stars.length();
        m_dirty = false;
    }

    if (instanceCount) {
        *instanceCount = m_instanceCount;
    }

    return m_instanceData;
}
