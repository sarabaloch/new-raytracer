#include "Basic.hpp"
#include "../world/World.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../materials/Material.hpp"

BasicTracer::BasicTracer(World* w) : Tracer(w) {}

RGBColor BasicTracer::trace_ray(const Ray& ray, int depth) const {
    ShadeInfo s = world_ptr->hit_objects(ray);
    if (s.hit)
        return s.material_ptr->shade(s);
    return world_ptr->bg_color;
}
