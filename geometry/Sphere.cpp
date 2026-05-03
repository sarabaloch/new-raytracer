#include "Sphere.hpp"

#include "../utilities/Ray.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../utilities/Vector3D.hpp"
#include "../utilities/BBox.hpp"
#include "../utilities/Constants.hpp"

#include <cmath>
#include <string>

// def
Sphere::Sphere() : c(Point3D(0.0, 0.0, 0.0)), r(0.0) {}

// constr with center and radius
Sphere::Sphere(const Point3D &center, float radius) : c(center), r(radius) {}

// copy
Sphere::Sphere(const Sphere &object) : Geometry(object), c(object.c), r(object.r) {}

Sphere &Sphere::operator=(const Sphere &rhs) {
    if (this != &rhs){
        Geometry::operator=(rhs);
        c = rhs.c;
        r = rhs.r;
    }

    return *this;
}

std::string Sphere::to_string() const {
    return "Sphere: center = " + c.to_string() + ", radius = " + std::to_string(r);
}

BBox Sphere::getBBox() const {
    // centre+-radius
    return BBox(
        Point3D(c.x - r, c.y - r, c.z - r),
        Point3D(c.x + r, c.y + r, c.z + r)
    );
}

// ray: o + t * d
// sphere: |p - c|^2 = r^2
// t^2(d . d) + 2t(d . (o - c)) + (o - c).(o - c) - r^2 = 0
bool Sphere::hit(const Ray &ray, float &t, ShadeInfo &s) const {
    Vector3D sub = ray.o - c;
    double a = ray.d * ray.d;
    double b = 2.0 * (ray.d * sub);
    double cc = sub * sub - (double)r*r;

    double quad = b * b - 4 * a * cc;
    if (quad < 0){
        return false; //no real roots
    }

    double sqrt_quad = sqrt(quad);
    double denominator = 2.0 * a;

    double root1 = (- b - sqrt_quad) / denominator;
    double root2 = (- b + sqrt_quad) / denominator;

    double res;
    if (root1 > kEpsilon) {
        res = root1;
    } else if (root2 > kEpsilon) {
        res = root2;  // ray began in sphere
    } else {
        return false;  // both behind
    }

    t = (float)res;

    // shading
    s.hit = true;
    s.material_ptr = material_ptr;
    s.hit_point = ray.o + res * ray.d;
    s.ray = ray;
    s.t = t;

    // outward norm = (hit_point - center) / radius
    Vector3D norm = s.hit_point - c;
    norm.normalize();
    s.normal = norm;

    return true;
}
