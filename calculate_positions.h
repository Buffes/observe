#ifndef POSITIONCALCULATOR_H
#define POSITIONCALCULATOR_H

#include <QList>
#include <QDateTime>
#include "datastructures.h"

namespace calc {
    QList<dVector3D> calculatePositions(QList<CelestialBody> bodies, QDateTime datetime);

    // Right ascension and declination expressed in radians.
    dVector3D RADeclinationToCartesian(double RA, double declination, double distance);
}

#endif // POSITIONCALCULATOR_H
