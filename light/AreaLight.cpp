#include "AreaLight.hpp"
#include "../utilities/ShadeInfo.hpp"
#include <cstdlib>
#include <cmath>

AreaLight::AreaLight(const Point3D&  corner,
                     const Vector3D& u,
                     const Vector3D& v,
                     const RGBColor& color,
                     float           intensity,
                     int             num_samples)
    : corner(corner), u(u), v(v),
      color(color), intensity(intensity),
      num_samples(num_samples)
{}

Point3D AreaLight::sample() const {
    float s = (float)rand() / (float)RAND_MAX;
    float t = (float)rand() / (float)RAND_MAX;
    return Point3D(
        corner.x + s * u.x + t * v.x,
        corner.y + s * u.y + t * v.y,
        corner.z + s * u.z + t * v.z
    );
}

Vector3D AreaLight::get_direction(const ShadeInfo& s) const {
    Point3D p = sample();
    Vector3D dir(
        p.x - s.hit_point.x,
        p.y - s.hit_point.y,
        p.z - s.hit_point.z
    );
    dir.normalize();
    return dir;
}

RGBColor AreaLight::L(const ShadeInfo&) const {
    return color * intensity;
}

float AreaLight::distance(const ShadeInfo& s) const {
    // Distance to centre of rectangle
    Point3D centre(
        corner.x + 0.5f * (u.x + v.x),
        corner.y + 0.5f * (u.y + v.y),
        corner.z + 0.5f * (u.z + v.z)
    );
    float dx = centre.x - s.hit_point.x;
    float dy = centre.y - s.hit_point.y;
    float dz = centre.z - s.hit_point.z;
    return sqrtf(dx*dx + dy*dy + dz*dz);
}
