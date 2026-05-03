#include "Geometry.hpp"
#include "../materials/Material.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../utilities/Ray.hpp"
#include "../utilities/Constants.hpp"

// We need a minimal World for ShadeInfo constructor; forward-declare a dummy
#include "../world/World.hpp"

Geometry::Geometry() : material_ptr(nullptr) {}

Material* Geometry::get_material() const {
    return material_ptr;
}

void Geometry::set_material(Material* mPtr) {
    material_ptr = mPtr;
}

// Default shadow_hit: reuse the hit() method
bool Geometry::shadow_hit(const Ray& ray, float max_distance) const {
    // We need a ShadeInfo but won't dereference it for shading
    // Create a dummy local world pointer trick: just call hit
    static World dummy_world;
    ShadeInfo s(dummy_world);
    float t;
    return hit(ray, t, s) && t > kEpsilon && t < max_distance;
}
