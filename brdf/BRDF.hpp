#pragma once
#include "../utilities/RGBColor.hpp"
#include "../utilities/Vector3D.hpp"

class ShadeInfo;

// Abstract BRDF base class
class BRDF {
public:
    virtual ~BRDF() = default;

    // Evaluate f(wi, wo) — the BRDF value given incoming and outgoing directions
    virtual RGBColor f(const ShadeInfo& s, const Vector3D& wi, const Vector3D& wo) const = 0;

    // Sample the BRDF: given wo, produce a wi and return f * cos_theta / pdf
    virtual RGBColor sample_f(const ShadeInfo& s, const Vector3D& wo, Vector3D& wi) const {
        wi = Vector3D(0, 1, 0);
        return RGBColor(0);
    }

    // Hemispherical-directional reflectance (rho)
    virtual RGBColor rho(const ShadeInfo& s, const Vector3D& wo) const {
        return RGBColor(0);
    }
};
