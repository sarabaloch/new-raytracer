#include "Point3D.hpp"
#include "Vector3D.hpp"

#include <cmath>
#include <string>

// 000
Point3D::Point3D() : x(0.0f), y(0.0f), z(0.0f) {}

//to c
Point3D::Point3D(float c) : x(c), y(c), z(c) {}

//each comp given
Point3D::Point3D(float x, float y, float z) : x(x), y(y), z(z) {}

std::string Point3D::to_string() const {
   return std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z);
}

//negation
Point3D Point3D::operator-() const {
    return Point3D(-x, -y, -z);
}

// from p to curr pt
Vector3D Point3D::operator-(const Point3D &p) const {
    return Vector3D(x - p.x, y - p.y, z - p.z);
}

// move pt by v and neg v
Point3D Point3D::operator+(const Vector3D &v) const {
    return Point3D(x + v.x, y + v.y, z + v.z);
}
Point3D Point3D::operator-(const Vector3D &v) const {
    return Point3D(x - v.x, y - v.y, z - v.z);
}

// scaling
Point3D Point3D::operator*(const float s) const {
    return Point3D(x * s, y * s, z * s);
}

//dist sq
float Point3D::d_squared(const Point3D &p) const {
    float dx = x - p.x;
    float dy = y - p.y;
    float dz = z - p.z;
    return dx*dx + dy*dy + dz*dz;
}
 
//dist
float Point3D::distance(const Point3D &p) const {
    return std::sqrt(d_squared(p));
}


// scale pt
Point3D operator*(const float a, const Point3D &pt) {
    return pt * a;
}

//min of each comp
Point3D min(const Point3D& a, const Point3D& b) {
    return Point3D(
        a.x < b.x ? a.x : b.x,
        a.y < b.y ? a.y : b.y,
        a.z < b.z ? a.z : b.z
    );
}

//max of each comp
Point3D max(const Point3D& a, const Point3D& b) {
    return Point3D(
        a.x > b.x ? a.x : b.x,
        a.y > b.y ? a.y : b.y,
        a.z > b.z ? a.z : b.z
    );
}