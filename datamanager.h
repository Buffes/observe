#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QString>
#include <QList>
#include "datastructures.h"

struct StarEntry {
    float id;
    double right_ascension;
    double declination;
    char spectral_type[2];
    int16_t magnitude;
    float proper_motion_ra;
    float proper_motion_decl;
    float scale;
};


/*
 * This class loads and holds the data that the other parts of the application need. It's a singleton because
 * the other C++ classes need to get a reference to this, but QML doesn'l allow parameters in constructors.
 * So they will fetch the DataManager instance from this class instead.
 */
class DataManager
{
public:
    static DataManager *getInstance();
    void loadBodies(QString path);
    void loadStarCatalog(QString path);

    int m_planet_count;
    QList<CelestialBody> m_planets; // We use the term "planet" here to also include the moon and the sun.
    QList<dVector3D> m_planet_positions;
    QList<StarEntry> m_stars; // Star catalog data
    QList<dVector3D> m_star_positions;
private:
    DataManager();

    static DataManager *instance;
};

#endif // DATAMANAGER_H
