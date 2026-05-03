#include "PointLight.hpp"
#include "../utilities/ShadeInfo.hpp"

PointLight::PointLight(const Point3D& p, const RGBColor& c, float i)
: position(p), color(c), intensity(i) {}

Vector3D PointLight::get_direction(const ShadeInfo& s) const {
    Vector3D dir = position - s.hit_point;
    dir.normalize();
    return dir;
}

RGBColor PointLight::L(const ShadeInfo&) const {
    return color * intensity;
}

float PointLight::distance(const ShadeInfo& s) const {
    return position.distance(s.hit_point);
}