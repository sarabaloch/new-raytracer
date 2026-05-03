#pragma once

#include "Acceleration.hpp"
#include "../geometry/Geometry.hpp"
#include <vector>

class BVH : public Acceleration {
    public:
        std::vector<Geometry*> objects;

        BVH();
        BVH(const std::vector<Geometry*>& objs);

        bool hit(const Ray& ray, float& t, ShadeInfo& s) const override;
};