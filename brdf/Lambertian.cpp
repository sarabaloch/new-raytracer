#include "Lambertian.hpp"
#include "../utilities/Constants.hpp"

Lambertian::Lambertian(const RGBColor& color, float k)
    : cd(color), kd(k) {}

// f = kd * cd / PI
RGBColor Lambertian::f(const ShadeInfo& s, const Vector3D& wi, const Vector3D& wo) const {
    return cd * kd * (float)invPI;
}

// rho = kd * cd
RGBColor Lambertian::rho(const ShadeInfo& s, const Vector3D& wo) const {
    return cd * kd;
}
