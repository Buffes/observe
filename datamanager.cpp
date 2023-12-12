#include "datamanager.h"
#include <QDebug>
#include <QFile>
#include "calculate_positions.h"

DataManager *DataManager::instance = NULL;

DataManager *DataManager::getInstance() {
    if (!instance) {
        instance = new DataManager();
    }
    return instance;
}

DataManager::DataManager() :
    m_planets(),
    m_planet_positions(),
    m_stars(),
    m_star_positions()
{
    loadStarCatalog("../observe/BSC5");
    loadBodies("../observe/orbital_elements.txt");
    m_planet_positions.reserve(m_planet_count);
}


void DataManager::loadBodies(QString path) {
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
                    this->m_planets.push_back(current_body);
                }
                current_body = {0};
                current_body.name = line.sliced(1, line.length() - 2);
                current_body.radius = 0.01f;
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
        this->m_planets.push_back(current_body); // last item
    }
    this->m_planet_count = this->m_planets.size();
}


template <typename T>
T readType(QByteArray buffer, size_t *index) {
    T value = *(T*)&buffer[*index];
    *index += sizeof(T);
    return value;
}


void DataManager::loadStarCatalog(QString path) {
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
        m_star_positions.reserve(num_stars);

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
            m_star_positions.append(calc::RADeclinationToCartesian(entry.right_ascension, entry.declination, 200.0));

        }
    }
}
