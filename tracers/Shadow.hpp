#pragma once
#include "Tracer.hpp"

class ShadowTracer : public Tracer {
public:
    ShadowTracer(World* w) : Tracer(w) {}
    RGBColor trace_ray(const Ray& ray, int depth) const override;
};
