#ifndef POSITIONCALCULATOR_H
#define POSITIONCALCULATOR_H

#include <QList>
#include <QDateTime>
#include "datastructures.h"

namespace calc {
    QList<dVector3D> calculatePositions(QList<CelestialBody> bodies, QDateTime datetime);
}

#endif // POSITIONCALCULATOR_H
