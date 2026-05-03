#pragma once
#include "../utilities/RGBColor.hpp"
#include "../utilities/Ray.hpp"

class World;

class Tracer {
public:
    World* world_ptr;

    Tracer(World* w);

    virtual RGBColor trace_ray(const Ray& ray, int depth) const = 0;
};