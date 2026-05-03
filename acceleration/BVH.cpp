#include "BVH.hpp"

BVH::BVH() {}

BVH::BVH(const std::vector<Geometry*>& objs)
    : objects(objs) {}

bool BVH::hit(const Ray& ray, float& t, ShadeInfo& s) const {
    bool hit_any = false;
    float t_min = 1e30f;
    float t_temp;

    for (Geometry* obj : objects) {
        ShadeInfo temp = s;

        if (obj->hit(ray, t_temp, temp) && t_temp < t_min) {
            hit_any = true;
            t_min = t_temp;
            s = temp;
        }
    }

    t = t_min;
    return hit_any;
}