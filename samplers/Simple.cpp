#include "Simple.hpp"

#include "../cameras/Camera.hpp"
#include "../utilities/Point3D.hpp"
#include "../utilities/Ray.hpp"
#include "../utilities/Vector3D.hpp"
#include "../world/ViewPlane.hpp"

Simple::Simple(Camera *c_ptr, ViewPlane *v_ptr) : Sampler(c_ptr, v_ptr) {}

// copy
Simple::Simple(const Simple &other) : Sampler(other) {}

Simple& Simple::operator=(const Simple &other) {
    if (this != &other) {
        Sampler::operator=(other);
    }

    return *this;
}

// origin at top left
// shoot one ray through cnetre of px, py
std::vector<Ray> Simple::get_rays(int px, int py) const {
    //getting vp size
    float total_width  = viewplane_ptr->bottom_right.x - viewplane_ptr->top_left.x;
    float total_height = viewplane_ptr->top_left.y - viewplane_ptr->bottom_right.y;

    //size of a pixel in this space
    float pixel_w = total_width  / (float)viewplane_ptr->hres;
    float pixel_h = total_height / (float)viewplane_ptr->vres;

    // center of pixel
    float world_x = viewplane_ptr->top_left.x + (px + 0.5f) * pixel_w;
    float world_y = viewplane_ptr->top_left.y - (py + 0.5f) * pixel_h;
    float world_z = viewplane_ptr->top_left.z;

    Point3D pixel_center(world_x, world_y, world_z);

    //get shoot direction according to if // or persp
    Vector3D dir = camera_ptr->get_direction(pixel_center);

    Ray ray(pixel_center, dir);
    ray.w = 1.0f;

    return {ray};
}