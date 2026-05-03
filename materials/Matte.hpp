#pragma once
#include "Material.hpp"
#include "../utilities/RGBColor.hpp"
#include "../brdf/Lambertian.hpp"

// Matte material: purely diffuse using Lambertian BRDF
class Matte : public Material {
public:
    Lambertian ambient_brdf;
    Lambertian diffuse_brdf;

    // c = diffuse color, kd = diffuse coefficient, ka = ambient coefficient
    Matte(const RGBColor& c, float kd, float ka = 0.15f);

    RGBColor shade(const ShadeInfo& s) const override;
};
