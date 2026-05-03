#pragma once
#include "Sampler.hpp"

// Jittered (stratified) sampler: divides each pixel into n×n subpixels
// and shoots one randomly-jittered ray per subpixel.
class Jittered : public Sampler {
public:
    int n;  // grid size: n*n samples per pixel

    Jittered(Camera* c_ptr, ViewPlane* v_ptr, int grid_n = 2);

    std::vector<Ray> get_rays(int px, int py) const override;

protected:
    // Get pixel-space world coordinates for a given sub-pixel
    std::vector<Ray> get_rays(float x, float y) const;
};
