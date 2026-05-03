/**
 * BVH.cpp — Real Bounding Volume Hierarchy
 *
 * Build: recursive median split along the longest axis of the centroid AABB.
 * Traverse: slab test on each node's bbox before recursing into children.
 * Leaf threshold: <= 4 primitives (tunable via MAX_LEAF).
 *
 * This replaces the previous linear-scan stub.
 */

#include "BVH.hpp"
#include "../utilities/Ray.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../utilities/BBox.hpp"

#include <algorithm>
#include <limits>
#include <cmath>

static const int MAX_LEAF = 4;  // max primitives in a leaf node

// ── helpers ────────────────────────────────────────────────────────────────

static float centroid(const BBox& b, int axis) {
    if (axis == 0) return 0.5f * (b.pmin.x + b.pmax.x);
    if (axis == 1) return 0.5f * (b.pmin.y + b.pmax.y);
    return             0.5f * (b.pmin.z + b.pmax.z);
}

static float bbox_extent(const BBox& b, int axis) {
    if (axis == 0) return b.pmax.x - b.pmin.x;
    if (axis == 1) return b.pmax.y - b.pmin.y;
    return             b.pmax.z - b.pmin.z;
}

// ── constructors ───────────────────────────────────────────────────────────

BVH::BVH() {}

BVH::BVH(const std::vector<Geometry*>& objs) : objects(objs) {
    if (objs.empty()) return;

    prims = objs;           // copy pointers; we'll reorder prims[] during build
    nodes.reserve(2 * (int)prims.size());
    build(0, (int)prims.size());
}

// ── build ──────────────────────────────────────────────────────────────────

int BVH::build(int start, int end) {
    // Allocate a new node
    nodes.push_back(BVHNode());
    int idx = (int)nodes.size() - 1;

    // Compute bounding box of all primitives in [start, end)
    BBox node_bbox = prims[start]->getBBox();
    for (int i = start + 1; i < end; i++)
        node_bbox.extend(prims[i]);
    nodes[idx].bbox = node_bbox;

    int count = end - start;

    // Leaf: few enough primitives, or degenerate range
    if (count <= MAX_LEAF) {
        nodes[idx].prim_start = start;
        nodes[idx].prim_count = count;
        return idx;
    }

    // Find the longest axis of the centroid bounding box
    // (splitting along the longest axis minimises wasted space)
    BBox centroid_bbox;
    centroid_bbox.pmin = Point3D( 1e30f,  1e30f,  1e30f);
    centroid_bbox.pmax = Point3D(-1e30f, -1e30f, -1e30f);

    for (int i = start; i < end; i++) {
        BBox b = prims[i]->getBBox();
        float cx = centroid(b, 0);
        float cy = centroid(b, 1);
        float cz = centroid(b, 2);
        if (cx < centroid_bbox.pmin.x) centroid_bbox.pmin.x = cx;
        if (cy < centroid_bbox.pmin.y) centroid_bbox.pmin.y = cy;
        if (cz < centroid_bbox.pmin.z) centroid_bbox.pmin.z = cz;
        if (cx > centroid_bbox.pmax.x) centroid_bbox.pmax.x = cx;
        if (cy > centroid_bbox.pmax.y) centroid_bbox.pmax.y = cy;
        if (cz > centroid_bbox.pmax.z) centroid_bbox.pmax.z = cz;
    }

    int axis = 0;
    float ext_x = bbox_extent(centroid_bbox, 0);
    float ext_y = bbox_extent(centroid_bbox, 1);
    float ext_z = bbox_extent(centroid_bbox, 2);
    if (ext_y > ext_x && ext_y >= ext_z) axis = 1;
    else if (ext_z > ext_x && ext_z > ext_y) axis = 2;

    // If all centroids are the same (degenerate), make a leaf
    float ext = bbox_extent(centroid_bbox, axis);
    if (ext < 1e-6f) {
        nodes[idx].prim_start = start;
        nodes[idx].prim_count = count;
        return idx;
    }

    // Median split: partition prims so left half has smaller centroid on axis
    int mid = (start + end) / 2;
    std::nth_element(
        prims.begin() + start,
        prims.begin() + mid,
        prims.begin() + end,
        [axis](Geometry* a, Geometry* b) {
            return centroid(a->getBBox(), axis) < centroid(b->getBBox(), axis);
        }
    );

    // Recurse — note: nodes[] may reallocate, so re-fetch idx after each call
    int left_idx  = build(start, mid);
    int right_idx = build(mid, end);

    // Re-fetch our node (vector may have reallocated during recursion)
    nodes[idx].left  = left_idx;
    nodes[idx].right = right_idx;
    nodes[idx].prim_start = 0;
    nodes[idx].prim_count = 0;

    return idx;
}

// ── traversal ─────────────────────────────────────────────────────────────

bool BVH::hit_node(int idx, const Ray& ray, float& t_max, ShadeInfo& s) const {
    const BVHNode& node = nodes[idx];

    // Bbox slab test — skip entire subtree if ray misses
    float t_enter, t_exit;
    if (!node.bbox.hit(ray, t_enter, t_exit)) return false;
    if (t_enter > t_max) return false;   // already found closer hit

    if (node.is_leaf()) {
        bool hit_any = false;
        for (int i = node.prim_start; i < node.prim_start + node.prim_count; i++) {
            float t_temp;
            ShadeInfo temp(s);          // same world ref
            if (prims[i]->hit(ray, t_temp, temp) && t_temp < t_max) {
                t_max   = t_temp;
                s       = temp;
                hit_any = true;
            }
        }
        return hit_any;
    }

    // Interior: test both children, closer first for early-out
    bool hit_any = false;
    hit_any |= hit_node(node.left,  ray, t_max, s);
    hit_any |= hit_node(node.right, ray, t_max, s);
    return hit_any;
}

bool BVH::hit(const Ray& ray, float& t, ShadeInfo& s) const {
    if (nodes.empty()) return false;
    float t_max = 1e30f;
    bool result = hit_node(0, ray, t_max, s);
    if (result) t = t_max;
    return result;
}