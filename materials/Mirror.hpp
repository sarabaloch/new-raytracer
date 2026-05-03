#pragma once
#include "Material.hpp"
#include "../utilities/RGBColor.hpp"
#include "../brdf/PerfectSpecular.hpp"

// Mirror (perfect specular) material
class Mirror : public Material {
public:
    PerfectSpecular specular_brdf;

    Mirror(float kr, const RGBColor& cr = RGBColor(1.0f));

    RGBColor shade(const ShadeInfo& s) const override;
};
