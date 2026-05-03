#include "PerfectSpecular.hpp"
#include "../utilities/ShadeInfo.hpp"

PerfectSpecular::PerfectSpecular(float reflectance, const RGBColor& color)
    : kr(reflectance), cr(color) {}

RGBColor PerfectSpecular::f(const ShadeInfo& s, const Vector3D& wi, const Vector3D& wo) const {
    return RGBColor(0); // delta distribution: f = 0 everywhere except the mirror direction
}

// wi = wo reflected about n; return kr * cr (BRDF * cos / pdf cancels)
RGBColor PerfectSpecular::sample_f(const ShadeInfo& s, const Vector3D& wo, Vector3D& wi) const {
    double ndotwo = s.normal * wo;
    wi = -wo + 2.0 * s.normal * ndotwo;
    return cr * kr;
}
