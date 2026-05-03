#include "Perspective.hpp"

#include "../utilities/Vector3D.hpp"
#include "../utilities/Point3D.hpp"

#include <cmath>

// def
Perspective::Perspective() : Camera(), pos(Point3D(0.0, 0.0, 0.0)) {}

// camera at scalar
Perspective::Perspective(float c) : Camera(), pos(Point3D(c, c, c)) {}

// cam xyz
Perspective::Perspective(float x, float y, float z) : Camera(), pos(Point3D(x, y, z)) {}

Perspective::Perspective(const Point3D &pt) : Camera(), pos(pt) {}

// copy
Perspective::Perspective(const Perspective &camera) : Camera(camera), pos(camera.pos) {}

// Assignment operator
Perspective &Perspective::operator=(const Perspective &other) {
    if (this != &other){
        Camera::operator=(other);
        pos = other.pos;
    }

    return *this;
}

// rays diverge as dir from camera to pixel pt
Vector3D Perspective::get_direction(const Point3D &p) const {
    Vector3D dir = p - pos; //camera position to pixel pt 
    dir.normalize();
    return dir;
}