#pragma once
#include "Light.hpp"
#include "../utilities/Point3D.hpp"
#include "../utilities/Vector3D.hpp"
#include "../utilities/RGBColor.hpp"

/**
 * AreaLight — a rectangular emitter.
 *
 * Defined by a corner point plus two edge vectors (u, v).
 * The rectangle spans: corner + s*u + t*v  for s,t in [0,1].
 *
 * get_direction() and distance() return values toward a NEW random
 * sample point each call, so the ShadowTracer naturally accumulates
 * soft shadows when it loops over multiple shadow rays.
 *
 * num_samples: how many shadow rays to cast per shading point.
 * More samples = smoother penumbra, slower render.
 */
class AreaLight : public Light {
public:
    Point3D  corner;      // one corner of the rectangle
    Vector3D u;           // first edge vector
    Vector3D v;           // second edge vector
    RGBColor color;
    float    intensity;
    int      num_samples; // shadow samples (8-16 is good)

    AreaLight(const Point3D&  corner,
              const Vector3D& u,
              const Vector3D& v,
              const RGBColor& color,
              float           intensity,
              int             num_samples = 12);

    // Returns direction toward a random point on the rectangle
    Vector3D get_direction(const ShadeInfo& s) const override;
    RGBColor L(const ShadeInfo& s) const override;
    float    distance(const ShadeInfo& s) const override;

    // Sample a random point on the rectangle
    Point3D sample() const;

    // True so ShadowTracer knows to use multi-sample soft shadow path
    virtual bool is_area_light() const { return true; }
};
