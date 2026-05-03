#include "Ray.hpp"

#include <string>

// def
Ray::Ray() : o(), d(), w(1.0f) {}

Ray::Ray(const Point3D &origin, const Vector3D &dir) : o(origin), d(dir), w(1.0f) {}


std::string Ray::to_string() const {
    return "Ray: origin =" + o.to_string() + ", direction =" + d.to_string() + "]";
}