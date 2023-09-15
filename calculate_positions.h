#ifndef POSITIONCALCULATOR_H
#define POSITIONCALCULATOR_H

#include <QList>
#include "datastructures.h"

namespace calc {
    QList<dVector3D> calculatePositions(QList<CelestialBody> bodies, int year, int month, int day, int hours, int minutes, int seconds);
}

#endif // POSITIONCALCULATOR_H
