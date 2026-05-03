#pragma once
#include "BRDF.hpp"
#include "../utilities/RGBColor.hpp"

// Phong glossy-specular BRDF: f = ks * (r . wo)^exp
class GlossySpecular : public BRDF {
public:
    float ks;      // specular coefficient
    float exp;     // Phong exponent (shininess)
    RGBColor cs;   // specular color (usually white)

    GlossySpecular(float specCoef, float shininess, const RGBColor& color);

    RGBColor f(const ShadeInfo& s, const Vector3D& wi, const Vector3D& wo) const override;
};
