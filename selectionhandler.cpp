#include "selectionhandler.h"
#include "el_math.h"

SelectionHandler::SelectionHandler() {
    this->data_manager = DataManager::getInstance();
}


bool ray_sphere_intersection(Vec3 ray_origin, Vec3 ray_direction, Vec3 sphere_pos, float sphere_radius) {
    Vec3 origin_to_sphere = sphere_pos - ray_origin;

    float origin_to_middle = dot(origin_to_sphere, ray_direction);
    float d_squared = dot(ray_direction, ray_direction) - origin_to_middle * origin_to_middle;
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

    //for (dVector3D &pos : data_manager->m_planet_positions)
}


