#include "Matte.hpp"
#include "../light/Light.hpp"
#include "../world/World.hpp"
#include "../utilities/ShadeInfo.hpp"

Matte::Matte(const RGBColor& c, float kd, float ka)
    : ambient_brdf(c, ka), diffuse_brdf(c, kd) {}

RGBColor Matte::shade(const ShadeInfo& s) const {
    Vector3D wo = -s.ray.d;  // outgoing direction toward camera
    // ambient term (constant, no light direction needed)
    RGBColor L = ambient_brdf.rho(s, wo) * RGBColor(1.0f);

    for (auto light : s.w->lights) {
        Vector3D wi = light->get_direction(s);
        float ndotwi = (float)(s.normal * wi);

        if (ndotwi > 0.0f) {
            L += diffuse_brdf.f(s, wi, wo) * light->L(s) * ndotwi;
        }
    }
    return L;
}
