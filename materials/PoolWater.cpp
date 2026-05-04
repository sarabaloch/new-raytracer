#include "PoolWater.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../utilities/Ray.hpp"
#include "../world/World.hpp"
#include "../tracers/Tracer.hpp"
#include <cmath>
#include <algorithm>

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

    // --- Transmitted ray (straight through into pool) ---
    Point3D  trans_origin = s.hit_point - s.normal * 0.002f;
    Ray      trans_ray(trans_origin, s.ray.d);
    RGBColor transmitted = s.w->tracer_ptr->trace_ray(trans_ray, s.depth + 1);

    // Fresnel-like blend: more reflective at grazing angles, more transparent
    // when looking straight down. ndotwo is cos(theta): 1=normal incidence, 0=grazing.
    float ndotwo  = (float)std::fabs(s.normal * wo);
    // Schlick approximation: F = F0 + (1-F0)*(1-cos)^5
    float F0      = reflect_weight;           // base reflectance at normal incidence
    float fresnel = F0 + (1.0f - F0) * powf(1.0f - ndotwo, 5.0f);
    fresnel       = std::min(fresnel, 0.85f); // cap so water never becomes a pure mirror

    return reflected  * fresnel
         + transmitted * water_tint * (1.0f - fresnel);
}