#include "Shadow.hpp"
#include "../world/World.hpp"
#include "../light/Light.hpp"
#include "../light/AreaLight.hpp"
#include "../geometry/Geometry.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../utilities/Ray.hpp"
#include "../materials/Material.hpp"
#include "../acceleration/Acceleration.hpp"
#include <cstdlib>
#include <cmath>

static bool is_occluded(const World* w, const Ray& shadow_ray, float dist_to_light) {
    if (w->use_acceleration && w->accel_ptr) {
        float t; ShadeInfo temp(*w);
        return (w->accel_ptr->hit(shadow_ray, t, temp) && t > 0.001f && t < dist_to_light);
    }
    for (auto obj : w->geometry) {
        float t; ShadeInfo temp(*w);
        if (obj->hit(shadow_ray, t, temp) && t > 0.001f && t < dist_to_light)
            return true;
    }
    return false;
}

RGBColor ShadowTracer::trace_ray(const Ray& ray, int depth) const {
    ShadeInfo s = world_ptr->hit_objects(ray);
    if (!s.hit) return world_ptr->bg_color;

    const_cast<ShadeInfo&>(s).depth = depth;

    if (s.material_ptr->is_reflective())
        return s.material_ptr->shade(s);

    RGBColor full_shade = s.material_ptr->shade(s);

    int   num_lights = (int)world_ptr->lights.size();
    float lit_accum  = 0.0f;
    float weight_sum = 0.0f;

    Point3D shadow_origin = s.hit_point + s.normal * 0.001f;

    for (auto light : world_ptr->lights) {
        Vector3D wi         = light->get_direction(s);
        float    ndotwi     = (float)(s.normal * wi);
        if (ndotwi <= 0.0f) continue;

        if (light->is_area_light()) {
            // ── AREA LIGHT: cast N shadow rays to random points on rectangle ──
            AreaLight* al = static_cast<AreaLight*>(light);
            int   hits    = 0;
            int   N       = al->num_samples;

            for (int i = 0; i < N; i++) {
                // Fresh random sample on the light rectangle each iteration
                Point3D sample = al->sample();
                Vector3D dir(
                    sample.x - s.hit_point.x,
                    sample.y - s.hit_point.y,
                    sample.z - s.hit_point.z
                );
                float dist = sqrtf((float)(dir * dir));
                dir.normalize();

                float ndotd = (float)(s.normal * dir);
                if (ndotd <= 0.0f) continue;

                Ray shadow_ray(shadow_origin, dir);
                if (!is_occluded(world_ptr, shadow_ray, dist))
                    hits++;
            }

            float shadow_factor = (float)hits / (float)N;
            lit_accum  += shadow_factor;
            weight_sum += 1.0f;

        } else {
            // ── POINT / DIRECTIONAL LIGHT: single hard shadow ray ──
            float dist_to_light = light->distance(s);
            Ray   shadow_ray(shadow_origin, wi);
            float shadow_factor = is_occluded(world_ptr, shadow_ray, dist_to_light) ? 0.0f : 1.0f;
            lit_accum  += shadow_factor;
            weight_sum += 1.0f;
        }
    }

    float lit_frac      = (weight_sum > 0.0f) ? (lit_accum / weight_sum) : 0.0f;
    float ambient_floor = 0.80f;
    float direct_scale  = (1.0f - ambient_floor) * lit_frac;

    return full_shade * (ambient_floor + direct_scale);
}
