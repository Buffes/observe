#ifndef POSITIONCALCULATOR_H
#define POSITIONCALCULATOR_H

#include <QObject>
#include <QQmlEngine>

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
};

// The sun is needed for other bodies, so we keep it as a constant
static CelestialBody sun {
    .name = "sun",
    .base_elements = {
        .N = 0.0,
        .i = 0.0,
        .w = 282.9404,
        .a = 1.000000,
        .e = 0.016709,
        .M = 356.0470,
    },
    .delta = {
        .N = 0.0,
        .i = 0.0,
        .w = +4.70935E-5,
        .a = 0.0,
        .e = -1.151E-9,
        .M = +0.9856002585,
    }
};

struct dVector3D {
    double x, y, z;
};

class PositionCalculator : public QObject {
    Q_OBJECT
    Q_PROPERTY(int bodyCount MEMBER m_bodyCount NOTIFY bodyCountChanged)
    Q_PROPERTY(QList<dVector3D> positions MEMBER m_positions NOTIFY positionsChanged)
    QML_ELEMENT
public:
    explicit PositionCalculator(QObject *parent = nullptr);
    int m_bodyCount;
    QList<CelestialBody> m_bodies;
    QList<dVector3D> m_positions;

public slots:
    void loadBodies(QString path);
    void calculatePosition(int year, int month, int day, int hours, int minutes, int seconds);
};

#endif // POSITIONCALCULATOR_H
