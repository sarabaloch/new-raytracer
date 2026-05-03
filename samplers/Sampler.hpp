#pragma once
#include <vector>
#include "../utilities/Ray.hpp"

class Camera;
class ViewPlane;

class Sampler {
public:
    Camera* camera_ptr;
    ViewPlane* viewplane_ptr;

    Sampler();
    Sampler(Camera* c_ptr, ViewPlane* v_ptr);
    virtual ~Sampler() = default;

    // Generate rays for pixel (px, py). Each ray has a weight w.
    virtual std::vector<Ray> get_rays(int px, int py) const = 0;
};
