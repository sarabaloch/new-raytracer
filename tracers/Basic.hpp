#pragma once

#include "Tracer.hpp"

class BasicTracer : public Tracer {
public:
    BasicTracer(World* w);

    RGBColor trace_ray(const Ray& ray, int depth) const override;
};