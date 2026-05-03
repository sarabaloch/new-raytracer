#include "DirectionalLight.hpp"

DirectionalLight::DirectionalLight(const Vector3D& d, const RGBColor& c)
    : dir(d), color(c) {
    dir.normalize();
}

Vector3D DirectionalLight::get_direction(const ShadeInfo&) const {
    return dir;
}

RGBColor DirectionalLight::L(const ShadeInfo&) const {
    return color;
}

float DirectionalLight::distance(const ShadeInfo&) const {
    return 1e8f; // effectively infinite
}