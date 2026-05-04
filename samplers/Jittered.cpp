#include "Jittered.hpp"
#include "../cameras/Camera.hpp"
#include "../world/ViewPlane.hpp"
#include "../utilities/Ray.hpp"
#include "../utilities/Point3D.hpp"
#include "../utilities/Vector3D.hpp"
#include <cstdlib>

Jittered::Jittered(Camera* c_ptr, ViewPlane* v_ptr, int grid_n)
    : Sampler(c_ptr, v_ptr), n(grid_n) {}

std::vector<Ray> Jittered::get_rays(int px, int py) const {
    float total_width  = viewplane_ptr->bottom_right.x - viewplane_ptr->top_left.x;
    float total_height = viewplane_ptr->top_left.y - viewplane_ptr->bottom_right.y;

    float pixel_w = total_width  / (float)viewplane_ptr->hres;
    float pixel_h = total_height / (float)viewplane_ptr->vres;

    float sub_w = pixel_w / (float)n;
    float sub_h = pixel_h / (float)n;
    float weight = 1.0f / (float)(n * n);

    std::vector<Ray> rays;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            // Random jitter within sub-cell [0,1)
            float rx = (float)rand() / (float)RAND_MAX;
            float ry = (float)rand() / (float)RAND_MAX;

            float world_x = viewplane_ptr->top_left.x + (px + (i + rx) / (float)n) * pixel_w;
            float world_y = viewplane_ptr->top_left.y - (py + (j + ry) / (float)n) * pixel_h;
            float world_z = viewplane_ptr->top_left.z;

            Point3D p(world_x, world_y, world_z);
            Vector3D dir = camera_ptr->get_direction(p);

            // True pinhole: ray starts at camera eye, travels through viewplane pixel
            Point3D cam_origin = camera_ptr->get_origin();
            Ray ray(cam_origin, dir);
            ray.w = weight;
            rays.push_back(ray);
        }
    }
    return rays;
}

// Unused override to satisfy Sampler base - world_x/y already computed above
std::vector<Ray> Jittered::get_rays(float x, float y) const {
    return {};
}