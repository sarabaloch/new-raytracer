#include "GlossySpecular.hpp"
#include "../utilities/ShadeInfo.hpp"
#include <cmath>

GlossySpecular::GlossySpecular(float specCoef, float shininess, const RGBColor& color)
    : ks(specCoef), exp(shininess), cs(color) {}

// r = -wi + 2(n . wi) * n
// f = ks * cs * (r . wo)^exp
RGBColor GlossySpecular::f(const ShadeInfo& s, const Vector3D& wi, const Vector3D& wo) const {
    Vector3D r = -wi + 2.0 * s.normal * (s.normal * wi);
    float rdotwo = (float)(r * wo);
    if (rdotwo > 0.0f)
        return cs * ks * std::pow(rdotwo, exp);
    return RGBColor(0);
}
