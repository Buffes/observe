#ifndef CALCULATEPOSITIONS_H
#define CALCULATEPOSITIONS_H

#include <QList>
#include <QDateTime>
#include "datastructures.h"

namespace calc {
    QList<dVector3D> calculatePositions(QList<CelestialBody> bodies, QDateTime datetime);
    dVector3D RADeclinationToCartesian(double RA, double declination, double distance); // Right ascension and declination expressed in radians.
    float magnitudeToScale(int16_t magnitude, int16_t max_magnitude = -124);
}

#endif // CALCULATEPOSITIONS_H
