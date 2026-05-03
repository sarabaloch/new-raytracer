#include "Shadow.hpp"
#include "../world/World.hpp"
#include "../light/Light.hpp"
#include "../geometry/Geometry.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../utilities/Ray.hpp"
#include "../materials/Material.hpp"

RGBColor ShadowTracer::trace_ray(const Ray& ray, int depth) const {
    ShadeInfo s = world_ptr->hit_objects(ray);

    if (!s.hit)
        return world_ptr->bg_color;

    const_cast<ShadeInfo&>(s).depth = depth;

    RGBColor result(0);
    // Ambient: always add ~15% of the material shade regardless of shadow
    result += s.material_ptr->shade(s) * 0.15f;

    for (auto light : world_ptr->lights) {
        Vector3D wi = light->get_direction(s);
        float dist_to_light = light->distance(s);

        // Offset origin along normal to avoid self-intersection
        Point3D shadow_origin = s.hit_point + s.normal * 0.01;
        Ray shadow_ray(shadow_origin, wi);

        bool in_shadow = false;
        for (auto obj : world_ptr->geometry) {
            float t;
            ShadeInfo temp(*world_ptr);
            if (obj->hit(shadow_ray, t, temp) && t > 0.001f && t < dist_to_light) {
                in_shadow = true;
                break;
            }
        }

        if (!in_shadow) {
            float ndotwi = (float)(s.normal * wi);
            if (ndotwi > 0.0f) {
                // Add 85% direct illumination (rest is ambient above)
                result += s.material_ptr->shade(s) * 0.85f;
            }
        }
    }

    return result;
}
