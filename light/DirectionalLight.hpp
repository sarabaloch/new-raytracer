#pragma once
#include "Light.hpp"

class DirectionalLight : public Light {
public:
    Vector3D dir;
    RGBColor color;

    DirectionalLight(const Vector3D& d, const RGBColor& c);

    Vector3D get_direction(const ShadeInfo& s) const override;
    RGBColor L(const ShadeInfo& s) const override;
    float distance(const ShadeInfo& s) const override;
};