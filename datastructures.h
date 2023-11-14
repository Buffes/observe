#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

#include <QString>
#include <QColor>

struct dVector3D {
    double x, y, z;
};

struct Color {
    uint16_t r, g, b;
};

struct Coords {
    double rightAscension;
    double declination;
};

struct OrbitalElements {
    double N; // longitude of ascending node
    double i; // inclination
    double w; // argument of perihelion
    double a; // semi-major axis (mean distance from the sun)
    double e; // eccentricity
    double M; // mean anomaly
};

struct CelestialBody {
    QString name;
    OrbitalElements base_elements;
    OrbitalElements delta; // change per day since J2000
    QColor color;
};

#endif // DATASTRUCTURES_H
