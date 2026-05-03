#pragma once
#include "Material.hpp"
#include "../utilities/RGBColor.hpp"
#include "../brdf/PerfectSpecular.hpp"

/**
 * PoolWater — semi-transparent mirror material.
 * Blends a perfect specular reflection with a straight-through (refracted)
 * ray that reveals geometry below (e.g. pool tiles).
 *
 *   shade = reflect_weight * reflected_color
 *         + (1 - reflect_weight) * transmitted_color * water_tint
 *
 * reflect_weight: 0 = fully transparent, 1 = fully mirror.
 */
class PoolWater : public Material {
public:
    PerfectSpecular specular_brdf;
    float           reflect_weight;  // how much reflection vs see-through
    RGBColor        water_tint;      // colour tint applied to transmitted ray

    PoolWater(float kr, float reflect_weight, const RGBColor& tint);
    RGBColor shade(const ShadeInfo& s) const override;
    bool is_reflective() const override { return true; }
};
