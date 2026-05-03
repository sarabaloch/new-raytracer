#include "Phong.hpp"
#include "../light/Light.hpp"
#include "../world/World.hpp"
#include "../utilities/ShadeInfo.hpp"

Phong::Phong(const RGBColor& c, float kd, float ks, float exp, float ka)
    : ambient_brdf(c, ka),
      diffuse_brdf(c, kd),
      specular_brdf(ks, exp, RGBColor(1.0f)) {}

RGBColor Phong::shade(const ShadeInfo& s) const {
    Vector3D wo = -s.ray.d;
    RGBColor L = ambient_brdf.rho(s, wo) * RGBColor(1.0f);

    for (auto light : s.w->lights) {
        Vector3D wi = light->get_direction(s);
        float ndotwi = (float)(s.normal * wi);

        if (ndotwi > 0.0f) {
            RGBColor diff = diffuse_brdf.f(s, wi, wo);
            RGBColor spec = specular_brdf.f(s, wi, wo);
            L += (diff + spec) * light->L(s) * ndotwi;
        }
    }
    return L;
}
