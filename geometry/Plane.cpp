#include "Plane.hpp"

#include "../utilities/Ray.hpp"
#include "../utilities/Constants.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../utilities/BBox.hpp"

#include <cmath>
#include <string>
#include <iostream>

// default: set to xz plane, y=0
Plane::Plane() : a(Point3D(0.0, 0.0, 0.0)), n(Vector3D(0.0, 1.0, 0.0)) {}

// constr w pt and n
Plane::Plane (const Point3D &pt, const Vector3D &normal) : Geometry(), a(pt), n(normal) {
    n.normalize();
}

// copy constr
Plane::Plane (const Plane &object) : Geometry(object), a(object.a), n(object.n) {}

// assignment op
Plane &Plane::operator=(const Plane &rhs) {
    if (this != &rhs) {
        Geometry::operator=(rhs);
        a = rhs.a;
        n = rhs.n;
    }

    return *this;
}

std::string Plane::to_string() const {
    return "Plane: point = " + a.to_string() + ", normal = " + n.to_string();
}

BBox Plane::getBBox() const {
    return BBox(Point3D(-kHugeValue), Point3D(kHugeValue));
}

// t = (a - o) . n / (d . n)
bool Plane::hit(const Ray &ray, float &t, ShadeInfo &s) const {
    double denominator = ray.d * n;
    Vector3D sub = a - ray.o;
    double result = (sub * n) / denominator;

    if (std::fabs(denominator) < kEpsilon) {
        return false; //if no hit
    }

    if (result < kEpsilon) { 
        return false; //if hit is behind the ray
    } 

    t = (float)result;

    // shading
    s.hit = true;
    s.material_ptr = material_ptr;
    s.hit_point = ray.o + result * ray.d;
    s.normal = n;
    s.ray = ray;
    s.t = t;

    return true;
}