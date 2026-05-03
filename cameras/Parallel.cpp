#include "Parallel.hpp"

#include "../utilities/Point3D.hpp"
#include "../utilities/Vector3D.hpp"
#include <cmath>

// def
Parallel::Parallel() : Camera(), dir(Vector3D(0.0, 0.0, -1.0)) {}

// constr with scalar
Parallel::Parallel(float c) : Camera(), dir(Vector3D(c, c, c)) {
    dir.normalize();
}

// constr with components
Parallel::Parallel(float x, float y, float z) : Camera(), dir(Vector3D(x, y, z)) {
    dir.normalize();
}

// constr with vector
Parallel::Parallel(const Vector3D &d) : Camera(), dir(d) {
    dir.normalize();
}

// copy
Parallel::Parallel(const Parallel &camera) : Camera(camera), dir(camera.dir) {}

Parallel &Parallel::operator=(const Parallel &other) {
    if (this != &other){
        Camera::operator=(other);
        dir = other.dir;
    }

    return *this;
}

// same dir for all pts as //
Vector3D Parallel::get_direction(const Point3D &p) const {
    return dir;
}