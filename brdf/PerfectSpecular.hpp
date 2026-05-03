#pragma once
#include "BRDF.hpp"
#include "../utilities/RGBColor.hpp"

// Perfect specular (mirror) BRDF
// Only contributes through sample_f (delta distribution)
class PerfectSpecular : public BRDF {
public:
    float kr;      // reflectance coefficient
    RGBColor cr;   // reflection color (usually white)

    PerfectSpecular(float reflectance, const RGBColor& color);

    // f = 0 for delta BRDFs (handled via sample_f)
    RGBColor f(const ShadeInfo& s, const Vector3D& wi, const Vector3D& wo) const override;

    // Returns reflected direction and the BRDF*cos/pdf = kr*cr
    RGBColor sample_f(const ShadeInfo& s, const Vector3D& wo, Vector3D& wi) const override;
};
