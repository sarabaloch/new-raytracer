#pragma once
#include "../utilities/Vector3D.hpp"
#include "../utilities/RGBColor.hpp"

class ShadeInfo;

class Light {
public:
    virtual ~Light() = default;

    virtual Vector3D get_direction(const ShadeInfo& s) const = 0;
    virtual RGBColor L(const ShadeInfo& s) const = 0;
    virtual float distance(const ShadeInfo& s) const = 0;
};