#include "World.hpp"

#include "../cameras/Camera.hpp"
#include "../geometry/Geometry.hpp"
#include "../samplers/Sampler.hpp"
#include "../utilities/Constants.hpp"
#include "../utilities/Ray.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../light/Light.hpp"
#include "../tracers/Tracer.hpp"
#include "../acceleration/Acceleration.hpp"

#include <vector>

World::World()
    : bg_color(0),
      camera_ptr(nullptr),
      sampler_ptr(nullptr),
      tracer_ptr(nullptr),
      accel_ptr(nullptr),
      use_acceleration(false)
{}

World::~World() {
    for (Geometry* g : geometry) delete g;
    for (Light*    l : lights)   delete l;

    delete sampler_ptr;
    delete camera_ptr;
    delete tracer_ptr;
    delete accel_ptr;
}

void World::add_geometry(Geometry* geom_ptr) {
    geometry.push_back(geom_ptr);
}

void World::set_camera(Camera* c_ptr) {
    camera_ptr = c_ptr;
}

ShadeInfo World::hit_objects(const Ray& ray) {
    ShadeInfo s(*this);
    float t;
    float mint = kHugeValue;

    // Use BVH if enabled; otherwise brute-force
    if (use_acceleration && accel_ptr) {
        accel_ptr->hit(ray, mint, s);
        return s;
    }

    for (Geometry* geom : geometry) {
        ShadeInfo temp(*this);
        if (geom->hit(ray, t, temp) && t < mint) {
            mint = t;
            s    = temp;
        }
    }

    return s;
}
