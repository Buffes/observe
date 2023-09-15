#include "datastructures.h"
#include "calculate_positions.h"
#include <QDebug>
#include <QVector3D>
#include <QtGlobal>
#include <QtMath>

double normalizeAngle(double angle) {
    if (angle >= 0) return fmod(angle, 360.0);
    else            return fmod(angle, 360.0) + 360;
}

// d is the day since 2000, calculated separately
OrbitalElements elements_for_day(CelestialBody body, int d) {
    OrbitalElements el;
    el = body.base_elements;
    el.N += d * body.delta.N;
    el.i += d * body.delta.i;
    el.w += d * body.delta.w;
    el.a += d * body.delta.a;
    el.e += d * body.delta.e;
    el.M += d * body.delta.M;

    // all angles need to be radians and in [0,2pi]
    el.N = qDegreesToRadians(normalizeAngle(el.N));
    el.i = qDegreesToRadians(normalizeAngle(el.i));
    el.w = qDegreesToRadians(normalizeAngle(el.w));
    el.M = qDegreesToRadians(normalizeAngle(el.M));

    return el;
}

// Method from Paul Schlyter: http://stjarnhimlen.se/comp/ppcomp.html#0
QList<dVector3D> calc::calculatePositions(QList<CelestialBody> bodies, int year, int month, int day, int hours, int minutes, int seconds) {
    QList<dVector3D> positions;

    // Calculate days since J2000. NOTE: this uses integer divisions
    double d = 367*year -
            7 * ( year + (month+9)/12 ) / 4 -
            3 * ( ( year + (month-9)/7 ) / 100 + 1 ) / 4 +
            275*month/9 + day - 730515;

    double decimal_hours = hours + minutes / 60.0 + seconds / 3600.0;
    d += decimal_hours / 24.0;
    qDebug() << "day: " << d;

    double ecliptic_obliquity = qDegreesToRadians(23.4393 - 3.563E-7 * d);

    // Compute for the sun first, because it is needed for the other bodies, and simpler to compute.
    OrbitalElements sun_el = elements_for_day(sun, d);
    double E_sun = sun_el.M + sun_el.e * qSin(sun_el.M) * (1.0 + sun_el.e * qCos(sun_el.M));

    double xv = qCos(E_sun) - sun_el.e;
    double yv = qSqrt(1.0 - sun_el.e*sun_el.e) * qSin(E_sun);

    double v_sun = qAtan2(yv, xv);
    double r_sun = qSqrt(xv*xv + yv*yv);
    double lon_sun = v_sun + sun_el.w;

    double xs = r_sun * qCos(lon_sun);
    double ys = r_sun * qSin(lon_sun);

    double xe = xs;
    double ye = ys * qCos(ecliptic_obliquity);
    double ze = ys * qSin(ecliptic_obliquity);

    positions.append({xe, ye, ze});
    //double RA  = qAtan2(ye, xe);
    //double dec = qAtan2(ze, qSqrt(xe*xe+ye*ye));
    //RA = qRadiansToDegrees(RA) / 15.0; // RA uses hours, mins, secs.
    //dec = qRadiansToDegrees(dec);

    // Compute all orbital elements first
    //QList<OrbitalElements> elements(this->bodies.size());
    QHash<QString, OrbitalElements> elements;
    for (int i = 0; i < bodies.size(); i++) {
        CelestialBody body = bodies[i];
        elements[body.name] = elements_for_day(body, d);
    }

    // Compute coordinates
    for (int i = 0; i < bodies.size(); i++) {
        const CelestialBody body = bodies[i];
        const OrbitalElements el = elements.value(body.name);

        if (el.e > 0.98) {
            qWarning() << "Too eccentric, we need to use formula for parabolic orbits";
        }
        // Solve Kepler's equation numerically for the eccentric anomaly E
        double E = el.M + el.e * qSin(el.M) * (1.0 + el.e * qCos(el.M));
        for (int i = 0; i < 100; i++) {
            double E_new = E - (E - el.e * qSin(E) - el.M) / (1 - el.e * qCos(E));
            if (qAbs(E_new - E) <= qDegreesToRadians(1E-7)) {
                E = E_new;
                break;
            }
            E = E_new;
        }

        double xv = el.a * (qCos(E) - el.e);
        double yv = el.a * (qSqrt(1.0 - el.e*el.e) * sin(E));

        double v = qAtan2(yv, xv);
        double r = qSqrt(xv*xv + yv*yv);

        double xh = r * (qCos(el.N) * qCos(v+el.w) - qSin(el.N) * qSin(v+el.w) * qCos(el.i));
        double yh = r * (qSin(el.N) * qCos(v+el.w) + qCos(el.N) * qSin(v+el.w) * qCos(el.i));
        double zh = r * (qSin(v+el.w) * qSin(el.i));

        double lon_ecl = qRadiansToDegrees(qAtan2(yh, xh));
        double lat_ecl = qRadiansToDegrees(qAtan2(zh, sqrt(xh*xh+yh*yh)));

        // Corrections for significant perturbations for certain bodies. NOTE: uses degrees
        if (body.name == "moon") {
            double lon_moon = el.M + el.w + el.N; // Mean longitude
            double D = lon_moon - lon_sun; // Mean elongation
            double F = lon_moon - el.N; // Argument of latitude

            lon_ecl -= 1.274 * qSin(el.M - 2*D);
            lon_ecl += 0.658 * qSin(2*D);
            lon_ecl -= 0.186 * qSin(sun_el.M);
            lon_ecl -= 0.059 * qSin(2*el.M - 2*D);
            lon_ecl -= 0.057 * qSin(el.M - 2*D + sun_el.M);
            lon_ecl += 0.053 * qSin(el.M + 2*D);
            lon_ecl += 0.046 * qSin(2*D - sun_el.M);
            lon_ecl += 0.041 * qSin(el.M - sun_el.M);
            lon_ecl -= 0.035 * qSin(D);
            lon_ecl -= 0.031 * qSin(el.M + sun_el.M);
            lon_ecl -= 0.015 * qSin(2*F - 2*D);
            lon_ecl += 0.011 * qSin(el.M - 4*D);
            lat_ecl -= 0.173 * qSin(F - 2*D);
            lat_ecl -= 0.055 * qSin(el.M - F - 2*D);
            lat_ecl -= 0.046 * qSin(el.M + F - 2*D);
            lat_ecl += 0.033 * qSin(F + 2*D);
            lat_ecl += 0.017 * qSin(2*el.M + F);
            r -= 0.58 * qCos(el.M - 2*D);
            r -= 0.46 * qCos(2*D);
        }
        else if (body.name == "jupiter") {
            double M_saturn = elements.value("saturn").M;
            lon_ecl -= 0.332 * qSin(2*el.M - 5*M_saturn - qDegreesToRadians(67.6));
            lon_ecl -= 0.056 * qSin(2*el.M - 2*M_saturn + qDegreesToRadians(21));
            lon_ecl += 0.042 * qSin(3*el.M - 5*M_saturn + qDegreesToRadians(21));
            lon_ecl -= 0.036 * qSin(el.M - 2*M_saturn);
            lon_ecl += 0.022 * qCos(el.M - M_saturn);
            lon_ecl += 0.023 * qSin(2*el.M - 3*M_saturn + qDegreesToRadians(52));
            lon_ecl -= 0.016 * qSin(el.M - 5*M_saturn - qDegreesToRadians(69));
        }
        else if (body.name == "saturn") {
            double M_jupiter = elements.value("jupiter").M;
            lon_ecl += 0.812 * qSin(2*M_jupiter - 5*el.M - qDegreesToRadians(67.6));
            lon_ecl -= 0.229 * qCos(2*M_jupiter - 4*el.M - qDegreesToRadians(2));
            lon_ecl += 0.119 * qSin(M_jupiter - 2*el.M - qDegreesToRadians(3));
            lon_ecl += 0.046 * qSin(2*M_jupiter - 6*el.M - qDegreesToRadians(69));
            lon_ecl += 0.014 * qSin(M_jupiter - 3*el.M + qDegreesToRadians(32));
            lat_ecl -= 0.020 * qCos(2*M_jupiter - 4*el.M - qDegreesToRadians(2));
            lat_ecl += 0.018 * qSin(2*M_jupiter - 6*el.M - qDegreesToRadians(49));
        }
        else if (body.name == "uranus") {
            double M_saturn = elements.value("saturn").M;
            double M_jupiter = elements.value("jupiter").M;
            lon_ecl += 0.040 * qSin(M_saturn - 2*el.M + 6);
            lon_ecl += 0.035 * qSin(M_saturn - 3*el.M + 33);
            lon_ecl -= 0.015 * qSin(M_jupiter - el.M + 20);
        }

        lon_ecl = qDegreesToRadians(lon_ecl);
        lat_ecl = qDegreesToRadians(lat_ecl);

        // heliocentric, ecliptic
        xh = r * qCos(lon_ecl) * qCos(lat_ecl);
        yh = r * qSin(lon_ecl) * qCos(lat_ecl);
        zh = r                 * qSin(lat_ecl);

        double xs = r_sun * cos(lon_sun);
        double ys = r_sun * sin(lon_sun);

        // geocentric, ecliptic
        double xg = xh + xs;
        double yg = yh + ys;
        double zg = zh;

        qDebug() << body.name << ": " << xg << ", " << yg << ", " << zg;

        positions.append({xg, yg, zg});
    }
    return positions;
}
