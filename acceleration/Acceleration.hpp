#pragma once
#include "../utilities/Ray.hpp"
#include "../utilities/ShadeInfo.hpp"

class Acceleration {
public:
    virtual bool hit(const Ray& ray, float& t, ShadeInfo& s) const = 0;
};