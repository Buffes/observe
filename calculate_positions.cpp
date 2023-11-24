#include "datastructures.h"
#include "calculate_positions.h"
#include <QDebug>
#include <QVector3D>
#include <QDateTime>
#include <QtGlobal>
#include <cmath>

#define TWO_PI 6.283185

double normalizeDegrees(double degrees) {
    if (degrees >= 0) return fmod(degrees, 360.0);
    else              return fmod(degrees, 360.0) + 360;
}

double normalizeRadians(double radians) {
    if (radians >= 0) return fmod(radians, TWO_PI);
    else              return fmod(radians, TWO_PI) + TWO_PI;
}

float clamp(float value, float min, float max) {
    return value < min ? min : (value > max ? max : value);
}

uint16_t clamp(uint16_t value, uint16_t min, uint16_t max) {
    return value < min ? min : (value > max ? max : value);
}

void printRightAscension(double RA_radians) {
    double RA_degrees = qRadiansToDegrees(normalizeRadians(RA_radians));
    double RA_seconds = RA_degrees * 240.0;
    int whole_hours   = (int)RA_seconds / 3600.0;
    RA_seconds -= whole_hours * 3600.0;
    int whole_minutes = (int)RA_seconds / 60.0;
    RA_seconds -= whole_minutes * 60.0;

    qDebug().nospace() << "RA: " << whole_hours << "h " << whole_minutes << "m " << RA_seconds << "s";
}

void printDeclination(double Decl_radians) {
    double decl_degrees = qRadiansToDegrees(normalizeRadians(Decl_radians));

    if (decl_degrees > 180.0) decl_degrees -= 360.0; // Declination is typically within [-90, 90] degrees.

    double whole_degrees, whole_arcminutes;
    double remainder = modf(decl_degrees, &whole_degrees);
    remainder = modf(remainder * 60.0, &whole_arcminutes);
    double arcseconds = remainder * 60.0;

    qDebug().nospace() << "Decl: " << whole_degrees << "° " << abs(whole_arcminutes) << "′ " << abs(arcseconds) << "″";
}


// d is the days since 2000 expressed as a decimal number, calculated separately.
OrbitalElements elements_for_day(CelestialBody body, double d) {
    OrbitalElements el;
    el = body.base_elements;
    el.N += d * body.delta.N;
    el.i += d * body.delta.i;
    el.w += d * body.delta.w;
    el.a += d * body.delta.a;
    el.e += d * body.delta.e;
    el.M += d * body.delta.M;

    // all angles need to be radians and in [0,2pi]
    el.N = qDegreesToRadians(normalizeDegrees(el.N));
    el.i = qDegreesToRadians(normalizeDegrees(el.i));
    el.w = qDegreesToRadians(normalizeDegrees(el.w));
    el.M = qDegreesToRadians(normalizeDegrees(el.M));

    return el;
}


// Method from Paul Schlyter: http://stjarnhimlen.se/comp/ppcomp.html#0
// Bodies is a list of CelestialBody, where the first is assumed to be the sun.
QList<dVector3D> calc::calculatePositions(QList<CelestialBody> bodies, QDateTime datetime) {
    int year  = datetime.date().year();
    int month = datetime.date().month();
    int day   = datetime.date().day();
    int hours = datetime.time().hour();
    int mins  = datetime.time().minute();
    int secs  = datetime.time().second();

    QList<dVector3D> positions;

    // Calculate days since J2000. NOTE: this uses integer divisions
    double d = 367*year -
            7 * ( year + (month+9)/12 ) / 4 -
            3 * ( ( year + (month-9)/7 ) / 100 + 1 ) / 4 +
            275*month/9 + day - 730515;

    double decimal_hours = (double)hours + (double)mins / 60.0 + (double)secs / 3600.0;
    d += decimal_hours / 24.0;

    double ecliptic_obliquity = qDegreesToRadians(23.4393 - 3.563E-7 * d);

    // Compute for the sun first, because it is needed for the other bodies, and simpler to compute.
    CelestialBody sun = bodies[0];
    OrbitalElements sun_el = elements_for_day(sun, d);
    double E_sun = sun_el.M + sun_el.e * sin(sun_el.M) * (1.0 + sun_el.e * cos(sun_el.M));

    double xv = cos(E_sun) - sun_el.e;
    double yv = sqrt(1.0 - sun_el.e*sun_el.e) * sin(E_sun);

    double v_sun = atan2(yv, xv);
    double r_sun = sqrt(xv*xv + yv*yv);
    double lon_sun = v_sun + sun_el.w;

    double xs = r_sun * cos(lon_sun);
    double ys = r_sun * sin(lon_sun);

    double xe = xs;
    double ye = ys * cos(ecliptic_obliquity);
    double ze = ys * sin(ecliptic_obliquity);

    positions.append({xe, ye, ze});
    //double RA  = atan2(ye, xe);
    //double dec = atan2(ze, sqrt(xe*xe+ye*ye));
    //RA = qRadiansToDegrees(RA) / 15.0; // RA uses hours, mins, secs.
    //dec = qRadiansToDegrees(dec);

    // Compute all orbital elements first
    QHash<QString, OrbitalElements> elements;
    for (int i = 1; i < bodies.size(); i++) {
        CelestialBody body = bodies[i];
        elements[body.name] = elements_for_day(body, d);
    }

    // Compute coordinates
    for (int i = 1; i < bodies.size(); i++) {
        const CelestialBody body = bodies[i];
        const OrbitalElements el = elements.value(body.name);

        if (el.e > 0.98) {
            qWarning() << "Too eccentric, we need to use formula for parabolic orbits";
            continue;
        }
        // Solve Kepler's equation numerically for the eccentric anomaly E
        double E = el.M + el.e * sin(el.M) * (1.0 + el.e * cos(el.M));
        for (int i = 0; i < 100; i++) {
            double E_new = E - (E - el.e * sin(E) - el.M) / (1.0 - el.e * cos(E));
            if (qAbs(E_new - E) <= qDegreesToRadians(1E-7)) {
                E = E_new;
                break;
            }
            E = E_new;
        }

        double xv = el.a * (cos(E) - el.e);
        double yv = el.a * (sqrt(1.0 - el.e*el.e) * sin(E));

        double v = atan2(yv, xv);
        double r = sqrt(xv*xv + yv*yv);

        double xh = r * (cos(el.N) * cos(v+el.w) - sin(el.N) * sin(v+el.w) * cos(el.i));
        double yh = r * (sin(el.N) * cos(v+el.w) + cos(el.N) * sin(v+el.w) * cos(el.i));
        double zh = r * (sin(v+el.w) * sin(el.i));

        // Corrections for significant perturbations for certain bodies.
        double lon_ecl = atan2(yh, xh);
        double lat_ecl = atan2(zh, sqrt(xh*xh+yh*yh));
        {
            if (body.name == "moon") {
                double sun_M = sun_el.M;
                double lon_moon = el.M + el.w + el.N; // Mean longitude
                double D = lon_moon - lon_sun; // Mean elongation
                double F = lon_moon - el.N; // Argument of latitude

                lon_ecl -= qDegreesToRadians(1.274) * sin(el.M - 2*D);
                lon_ecl += qDegreesToRadians(0.658) * sin(2*D);
                lon_ecl -= qDegreesToRadians(0.186) * sin(sun_M);
                lon_ecl -= qDegreesToRadians(0.059) * sin(2*el.M - 2*D);
                lon_ecl -= qDegreesToRadians(0.057) * sin(el.M - 2*D + sun_M);
                lon_ecl += qDegreesToRadians(0.053) * sin(el.M + 2*D);
                lon_ecl += qDegreesToRadians(0.046) * sin(2*D - sun_M);
                lon_ecl += qDegreesToRadians(0.041) * sin(el.M - sun_M);
                lon_ecl -= qDegreesToRadians(0.035) * sin(D);
                lon_ecl -= qDegreesToRadians(0.031) * sin(el.M + sun_M);
                lon_ecl -= qDegreesToRadians(0.015) * sin(2*F - 2*D);
                lon_ecl += qDegreesToRadians(0.011) * sin(el.M - 4*D);
                lat_ecl -= qDegreesToRadians(0.173) * sin(F - 2*D);
                lat_ecl -= qDegreesToRadians(0.055) * sin(el.M - F - 2*D);
                lat_ecl -= qDegreesToRadians(0.046) * sin(el.M + F - 2*D);
                lat_ecl += qDegreesToRadians(0.033) * sin(F + 2*D);
                lat_ecl += qDegreesToRadians(0.017) * sin(2*el.M + F);
                r -= qDegreesToRadians(0.58) * cos(el.M - 2*D);
                r -= qDegreesToRadians(0.46) * cos(2*D);
            }
            else if (body.name == "jupiter") {
                double M_saturn = elements.value("saturn").M;
                lon_ecl -= qDegreesToRadians(0.332) * sin(2*el.M - 5*M_saturn - qDegreesToRadians(67.6));
                lon_ecl -= qDegreesToRadians(0.056) * sin(2*el.M - 2*M_saturn + qDegreesToRadians(21));
                lon_ecl += qDegreesToRadians(0.042) * sin(3*el.M - 5*M_saturn + qDegreesToRadians(21));
                lon_ecl -= qDegreesToRadians(0.036) * sin(el.M - 2*M_saturn);
                lon_ecl += qDegreesToRadians(0.022) * cos(el.M - M_saturn);
                lon_ecl += qDegreesToRadians(0.023) * sin(2*el.M - 3*M_saturn + qDegreesToRadians(52));
                lon_ecl -= qDegreesToRadians(0.016) * sin(el.M - 5*M_saturn - qDegreesToRadians(69));
            }
            else if (body.name == "saturn") {
                double M_jupiter = elements.value("jupiter").M;
                lon_ecl += qDegreesToRadians(0.812) * sin(2*M_jupiter - 5*el.M - qDegreesToRadians(67.6));
                lon_ecl -= qDegreesToRadians(0.229) * cos(2*M_jupiter - 4*el.M - qDegreesToRadians(2));
                lon_ecl += qDegreesToRadians(0.119) * sin(M_jupiter - 2*el.M - qDegreesToRadians(3));
                lon_ecl += qDegreesToRadians(0.046) * sin(2*M_jupiter - 6*el.M - qDegreesToRadians(69));
                lon_ecl += qDegreesToRadians(0.014) * sin(M_jupiter - 3*el.M + qDegreesToRadians(32));
                lat_ecl -= qDegreesToRadians(0.020) * cos(2*M_jupiter - 4*el.M - qDegreesToRadians(2));
                lat_ecl += qDegreesToRadians(0.018) * sin(2*M_jupiter - 6*el.M - qDegreesToRadians(49));
            }
            else if (body.name == "uranus") {
                double M_saturn = elements.value("saturn").M;
                double M_jupiter = elements.value("jupiter").M;
                lon_ecl += qDegreesToRadians(0.040) * sin(M_saturn - 2*el.M + qDegreesToRadians(6));
                lon_ecl += qDegreesToRadians(0.035) * sin(M_saturn - 3*el.M + qDegreesToRadians(33));
                lon_ecl -= qDegreesToRadians(0.015) * sin(M_jupiter - el.M + qDegreesToRadians(20));
            }
        }

        // heliocentric, ecliptic. For the moon this is geocentric.
        xh = r * cos(lon_ecl) * cos(lat_ecl);
        yh = r * sin(lon_ecl) * cos(lat_ecl);
        zh = r                 * sin(lat_ecl);

        double xg;
        double yg;
        double zg;

        if (body.name == "moon") {
            // The moon is already in geocentric. We just convert from Earth radii to AU.
            xg = xh * 4.258750455597227e-5;
            yg = yh * 4.258750455597227e-5;
            zg = zh * 4.258750455597227e-5;
        }
        else {
            double xs = r_sun * cos(lon_sun);
            double ys = r_sun * sin(lon_sun);

            // geocentric, ecliptic
            xg = xh + xs;
            yg = yh + ys;
            zg = zh;
        }

        // geocentric, equatorial
        double xe = xg;
        double ye = yg * cos(ecliptic_obliquity) - zg * sin(ecliptic_obliquity);
        double ze = yg * sin(ecliptic_obliquity) + zg * cos(ecliptic_obliquity);

        double len = sqrt(xe*xe + ye*ye + ze*ze);
        xe *= 1.0 / len;
        ye *= 1.0 / len;
        ze *= 1.0 / len;

        //qDebug() << body.name << ": " << xg << ", " << yg << ", " << zg;

        positions.append({xe, ye, ze});

        //if (body.name == "jupiter") {
            double RA   = atan2(ye, xe);
            double decl = atan2(ze, sqrt(xe*xe + ye*ye));
            qDebug() << body.name;
            printRightAscension(RA);
            printDeclination(decl);
            qDebug() << "";
        //}
    }
    return positions;
}


dVector3D calc::RADeclinationToCartesian(double RA, double declination, double distance) {
    // NOTE: For OpenGL compatibility we want a right handed system with Y axis as up.
    // The coordinates we get are in a RHS with Z axis up. Thus we rotate by 90 degrees
    // around the X axis, which is the same as swapping Y and Z, and then negating Z.
    dVector3D result = {
        .x = distance * cos(declination) * cos(RA),
        .y = distance * sin(declination),
        .z = -distance * cos(declination) * sin(RA)
    };

    return result;
}

float calc::magnitudeToScale(int16_t magnitude, int16_t max_magnitude) {
    // The magnitude scale is inverse logarithmic. We set a reference size for magnitude 1,
    // and then calculate the color from the difference in magnitude.

    float start_size = 0.02f;
    int16_t magnitude_diff = 100 - magnitude; // NOTE: the 16 bit number is 100 times the actual magnitude.

    float brightness_diff = powf(10.0f, 0.01f * 0.4f * magnitude_diff);
    float max_brightness_diff = powf(10.0f, 0.01f * 0.4f * (100 - max_magnitude));

    float min = 0.004f;
    float max = 5.0f;
    float range = max - min;

    float result = start_size * brightness_diff * (range / max_brightness_diff) + min;
    return result;
}
