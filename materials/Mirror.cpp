#include "Mirror.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../utilities/Ray.hpp"
#include "../world/World.hpp"
#include "../tracers/Tracer.hpp"

Mirror::Mirror(float kr, const RGBColor& cr)
    : specular_brdf(kr, cr) {}

RGBColor Mirror::shade(const ShadeInfo& s) const {
    if (s.depth >= 5) return RGBColor(0); // recursion limit

    Vector3D wo = -s.ray.d;
    Vector3D wi;
    RGBColor fr = specular_brdf.sample_f(s, wo, wi);

    // Offset the hit point slightly to avoid self-intersection
    Point3D origin = s.hit_point + s.normal * 0.001;
    Ray reflected(origin, wi);

    return fr * s.w->tracer_ptr->trace_ray(reflected, s.depth + 1);
}
