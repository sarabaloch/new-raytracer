#include "BBox.hpp"
#include "Point3D.hpp"
#include "Ray.hpp"
#include "../geometry/Geometry.hpp"

#include <algorithm>
#include <cmath>
#include <string>

BBox::BBox(const Point3D& min, const Point3D& max) : pmin(min), pmax(max) {}


std::string BBox::to_string() const {
     return "BBox: min(" + pmin.to_string() + ") , max (" + pmax.to_string() + ")";
}


// slab
//checks if bbox hit
bool BBox::hit(const Ray &ray, float &t_enter, float &t_exit) const {
    float tx_min, ty_min, tz_min, tx_max, ty_max, tz_max;

    //x
    float inv_dx = 1.0f / (float)ray.d.x;
    if (inv_dx >= 0) {
        tx_min = (pmin.x - ray.o.x) * inv_dx;
        tx_max = (pmax.x - ray.o.x) * inv_dx;
    } else {
        tx_min = (pmax.x - ray.o.x) * inv_dx;
        tx_max = (pmin.x - ray.o.x) * inv_dx;
    }

    //y
    float inv_dy = 1.0f / ray.d.y;
    if (inv_dy >= 0) {
        ty_min = (pmin.y - ray.o.y) * inv_dy;
        ty_max = (pmax.y - ray.o.y) * inv_dy;
    } else {
        ty_min = (pmax.y - ray.o.y) * inv_dy;
        ty_max = (pmin.y - ray.o.y) * inv_dy;
    }

    //z
    float inv_dz = 1.0f / ray.d.z;
    if (inv_dz >= 0) {
        tz_min = (pmin.z - ray.o.z) * inv_dz;
        tz_max = (pmax.z - ray.o.z) * inv_dz;
    } else {
        tz_min = (pmax.z - ray.o.z) * inv_dz;
        tz_max = (pmin.z - ray.o.z) * inv_dz;
    }

    //checking for overlap
    t_enter = std::max({tx_min, ty_min, tz_min});
    t_exit  = std::min({tx_max, ty_max, tz_max});

    return t_enter <= t_exit;
}


// extend to cover geometry
void BBox::extend(Geometry* g) {
    BBox b = g->getBBox();
    extend(b);
}


// extend w another bbox
void BBox::extend(const BBox& b) {
    pmin = min(pmin, b.pmin);
    pmax = max(pmax, b.pmax);
}


// checks if pt in bbox
bool BBox::contains(const Point3D& p) {
    return (p.x >= pmin.x && p.x <= pmax.x) &&
           (p.y >= pmin.y && p.y <= pmax.y) &&
           (p.z >= pmin.z && p.z <= pmax.z);
}


// chcks overlap w geometry
bool BBox::overlaps(Geometry* g) {
    return overlaps(g->getBBox());
}


// chcks overlap w another bbox
bool BBox::overlaps(const BBox& b) {
    return (pmin.x <= b.pmax.x && pmax.x >= b.pmin.x) &&
           (pmin.y <= b.pmax.y && pmax.y >= b.pmin.y) &&
           (pmin.z <= b.pmax.z && pmax.z >= b.pmin.z);
}