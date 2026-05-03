#include "Cosine.hpp"

#include "../utilities/ShadeInfo.hpp"
#include "../utilities/Vector3D.hpp"

// def
Cosine::Cosine() : color(0.0f, 0.0f, 0.0f) {}

// grey
Cosine::Cosine(float c) : color(c, c, c) {}

// RGB
Cosine::Cosine(float r, float g, float b) : color(r, g, b) {}

Cosine::Cosine(const RGBColor &c) : color(c) {}

// copy
Cosine::Cosine(const Cosine &other) : Material(other), color(other.color) {}

Cosine& Cosine::operator=(const Cosine &other) {
    if (this != &other) {
        Material::operator=(other);
        color = other.color;
    }

    return *this;
}

// shade = color * costheta
//ctheta = n . -ray.d
RGBColor Cosine::shade(const ShadeInfo &sinfo) const {
    Vector3D normal = sinfo.normal; 
    Vector3D ray_dir = sinfo.ray.d; //incoming
    double cos_theta = normal * (-ray_dir);

    // clamp to 0 so no negative lighting
    if (cos_theta < 0.0) {
        cos_theta = 0.0;
    }

    return color * (float)cos_theta;
}