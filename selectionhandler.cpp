#include "selectionhandler.h"
#include <QDebug>

SelectionHandler::SelectionHandler() {
    this->data_manager = DataManager::getInstance();
}


bool raySphereIntersection(QVector3D ray_origin, QVector3D ray_direction, QVector3D sphere_pos, float sphere_radius) {
    QVector3D origin_to_sphere = sphere_pos - ray_origin;

    float origin_to_middle = QVector3D::dotProduct(origin_to_sphere, ray_direction);
    float d_squared = QVector3D::dotProduct(origin_to_sphere, origin_to_sphere) - origin_to_middle * origin_to_middle;
    float sphere_radius_squared = sphere_radius * sphere_radius;

    return d_squared <= sphere_radius_squared;

    /*
    float middle_to_entry = sqrt(sphere_radius_squared - d_squared);
    float entry_point = origin_to_middle - middle_to_entry;
    float exit_point  = origin_to_middle + middle_to_entry;
    */
}

void SelectionHandler::rayPick(QVector3D origin, QVector3D direction) {
    direction.normalize();

    for (int i = 0; i < data_manager->m_planet_count; i++) {
        dVector3D pos = data_manager->m_planet_positions[i];
        float radius  = data_manager->m_planets[i].radius;
        if (raySphereIntersection(origin, direction, QVector3D(pos.x, pos.y, pos.z), radius))
            qDebug() << "hit" << data_manager->m_planets[i].name;
    }
}


