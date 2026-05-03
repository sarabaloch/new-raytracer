#pragma once
#include "BRDF.hpp"
#include "../utilities/RGBColor.hpp"
#include "../utilities/Constants.hpp"

// Lambertian diffuse BRDF: f = kd * cd / PI
class Lambertian : public BRDF {
public:
    float kd;      // diffuse coefficient [0,1]
    RGBColor cd;   // diffuse color

    Lambertian(const RGBColor& color, float k);

    RGBColor f(const ShadeInfo& s, const Vector3D& wi, const Vector3D& wo) const override;
    RGBColor rho(const ShadeInfo& s, const Vector3D& wo) const override;
};
