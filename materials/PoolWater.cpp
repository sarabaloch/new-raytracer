#include "PoolWater.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../utilities/Ray.hpp"
#include "../world/World.hpp"
#include "../tracers/Tracer.hpp"

PoolWater::PoolWater(float kr, float rw, const RGBColor& tint)
    : specular_brdf(kr, RGBColor(1.0f)),
      reflect_weight(rw),
      water_tint(tint) {}

RGBColor PoolWater::shade(const ShadeInfo& s) const {
    if (s.depth >= 5) return water_tint;

    Vector3D wo = -s.ray.d;
    Vector3D wi;
    specular_brdf.sample_f(s, wo, wi);

    // --- Reflected ray (top surface mirror) ---
    Point3D  reflect_origin = s.hit_point + s.normal * 0.002f;
    Ray      reflect_ray(reflect_origin, wi);
    RGBColor reflected = s.w->tracer_ptr->trace_ray(reflect_ray, s.depth + 1);

    // --- Transmitted ray (straight through, slightly refracted downward) ---
    // Simple approximation: continue ray direction unchanged (no bend).
    // This lets us see tiles directly below the water surface.
    Point3D  trans_origin = s.hit_point - s.normal * 0.002f;
    Ray      trans_ray(trans_origin, s.ray.d);  // same direction, passes through
    RGBColor transmitted = s.w->tracer_ptr->trace_ray(trans_ray, s.depth + 1);

    // Blend: reflection on top, tiles visible underneath
    return reflected * reflect_weight
         + transmitted * water_tint * (1.0f - reflect_weight);
}
