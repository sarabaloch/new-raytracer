#pragma once
#include "Material.hpp"
#include "../utilities/RGBColor.hpp"
#include "../brdf/Lambertian.hpp"
#include "../brdf/GlossySpecular.hpp"

// Phong material: diffuse + glossy-specular BRDFs
class Phong : public Material {
public:
    Lambertian    ambient_brdf;
    Lambertian    diffuse_brdf;
    GlossySpecular specular_brdf;

    // c = base color, kd = diffuse coeff, ks = specular coeff, exp = shininess
    Phong(const RGBColor& c, float kd, float ks, float exp, float ka = 0.1f);

    RGBColor shade(const ShadeInfo& s) const override;
};
