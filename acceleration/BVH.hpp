#pragma once

#include "Acceleration.hpp"
#include "../geometry/Geometry.hpp"
#include "../utilities/BBox.hpp"
#include <vector>

// Internal BVH node — either a leaf (holds primitives) or an interior
// node (holds two children split along an axis).
struct BVHNode {
    BBox  bbox;
    int   left  = -1;   // index into nodes[], -1 = none
    int   right = -1;
    // Leaf data: range in the sorted primitives array
    int   prim_start = 0;
    int   prim_count = 0;

    bool is_leaf() const { return left == -1 && right == -1; }
};

class BVH : public Acceleration {
public:
    // Original list kept for World compatibility
    std::vector<Geometry*> objects;

    BVH();
    BVH(const std::vector<Geometry*>& objs);
    ~BVH() = default;

    bool hit(const Ray& ray, float& t, ShadeInfo& s) const override;

private:
    std::vector<BVHNode>   nodes;
    std::vector<Geometry*> prims;  // reordered during build

    // Build a node covering prims[start..end), return its index in nodes[]
    int build(int start, int end);

    // Recursive traversal
    bool hit_node(int idx, const Ray& ray, float& t_max, ShadeInfo& s) const;
};