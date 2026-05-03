#include "ShadeInfo.hpp"
#include "../world/World.hpp"

// initialize with no hit, null material, zero values
ShadeInfo::ShadeInfo(const World &wr) : hit(false), material_ptr(nullptr), hit_point(), normal(), ray(), depth(0), t(0.0f), w(&wr) {}
