#include "Triangle.hpp"

#include "../utilities/Ray.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../utilities/Vector3D.hpp"
#include "../utilities/BBox.hpp"
#include "../utilities/Constants.hpp"

#include <string>
#include <cmath>
#include <algorithm>

// def
Triangle::Triangle() : v0(Point3D(0,0,0)), v1(Point3D(0,0,0)), v2(Point3D(0,0,0)) {}

// constr with vertices
Triangle::Triangle(const Point3D &a, const Point3D &b, const Point3D &c) : v0(a), v1(b), v2(c) {}

// copy
Triangle::Triangle(const Triangle &object) : Geometry(object), v0(object.v0), v1(object.v1), v2(object.v2) {}

Triangle &Triangle::operator=(const Triangle &rhs) {
    if (this != &rhs){
        Geometry::operator=(rhs);
        v0 = rhs.v0;
        v1 = rhs.v1;
        v2 = rhs.v2;
    }

    return *this;
}

std::string Triangle::to_string() const {
    return "Triangle: v0 = " + v0.to_string() + ", v1 = " + v1.to_string() + ", v2 = " + v2.to_string();
}

//minmax of each coord
BBox Triangle::getBBox() const {
    float minx = std::min({v0.x, v1.x, v2.x});
    float miny = std::min({v0.y, v1.y, v2.y});
    float minz = std::min({v0.z, v1.z, v2.z});

    float maxx = std::max({v0.x, v1.x, v2.x});
    float maxy = std::max({v0.y, v1.y, v2.y});
    float maxz = std::max({v0.z, v1.z, v2.z});

    return BBox(Point3D(minx, miny, minz), Point3D(maxx, maxy, maxz));
}

// with the help of barycentric and then see if in triangle
bool Triangle::hit(const Ray &ray, float &t, ShadeInfo &s) const {
    Vector3D edge1 = v1 - v0;
    Vector3D edge2 = v2 - v0;
    Vector3D h = ray.d ^ edge2;      // d x edge2
    double a = edge1 * h;             // edge1 . h

    // ray // to tri
    if (std::fabs(a) < kEpsilon) {
        return false;
    }

    double b = 1.0 / a;
    Vector3D s_vec = ray.o - v0; //from v0 to origin

    double u = b * (s_vec * h); //bary coord
    if (u < 0.0 || u > 1.0){
        return false;
    }

    Vector3D q = s_vec ^ edge1; // s x edge1
    double v = b * (ray.d * q); //bary coord
    if (v < 0.0 || (u + v) > 1.0){
        return false;
    }

    double res = b * (edge2 * q);
    if (res < kEpsilon) { 
        return false; 
    }

    t = (float)res;

    // shading
    s.hit = true;
    s.material_ptr = material_ptr;
    s.hit_point = ray.o + res * ray.d;
    s.ray = ray;
    s.t = t;

    Vector3D normal_vec = edge1 ^ edge2;
    normal_vec.normalize();
    s.normal = normal_vec;

    return true;
}