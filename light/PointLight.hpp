#pragma once

#include "Light.hpp"
#include "../utilities/Point3D.hpp"
#include "../utilities/Vector3D.hpp"

class PointLight : public Light {
public:
    Point3D position;
    RGBColor color;
    float intensity;

    PointLight(const Point3D& p, const RGBColor& c, float i);

    Vector3D get_direction(const ShadeInfo& s) const override;
    RGBColor L(const ShadeInfo& s) const override;
    float distance(const ShadeInfo& s) const override;
};