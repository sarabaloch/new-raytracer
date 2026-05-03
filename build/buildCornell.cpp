/**
 * Cornell Box scene build file.
 *
 * Features used:
 *   - Perspective camera
 *   - Two lights: PointLight (main) + DirectionalLight (fill)
 *   - Matte (diffuse), Phong (glossy), Mirror (specular) materials
 *   - ShadowTracer with shadow rays
 *   - Jittered sampler (4x4 = 16 samples per pixel)
 *   - BVH acceleration (controlled by world.use_acceleration)
 */

#include "../cameras/Perspective.hpp"

#include "../geometry/Plane.hpp"
#include "../geometry/Sphere.hpp"
#include "../geometry/Triangle.hpp"

#include "../materials/Cosine.hpp"
#include "../materials/Matte.hpp"
#include "../materials/Phong.hpp"
#include "../materials/Mirror.hpp"

#include "../samplers/Jittered.hpp"
#include "../samplers/Simple.hpp"

#include "../light/PointLight.hpp"
#include "../light/DirectionalLight.hpp"

#include "../tracers/Shadow.hpp"
#include "../tracers/Basic.hpp"

#include "../acceleration/BVH.hpp"

#include "../utilities/Constants.hpp"

#include "../world/World.hpp"

void World::build(void) {
    // =====================
    // VIEW PLANE
    // =====================
    vplane.top_left     = Point3D(-200, 200, 100);
    vplane.bottom_right = Point3D( 200,-200, 100);
    vplane.hres = 500;
    vplane.vres = 500;

    bg_color = RGBColor(0);

    // =====================
    // CAMERA
    // =====================
    set_camera(new Perspective(Point3D(0, 0, 550)));

    // =====================
    // SAMPLER (jittered 3x3 anti-aliasing)
    // =====================
    sampler_ptr = new Jittered(camera_ptr, &vplane, 3);

    // =====================
    // TRACERS
    // =====================
    tracer_ptr = new ShadowTracer(this);

    // =====================
    // LIGHTS
    // =====================
    // Main area-approximated point light on ceiling
    add_light(new PointLight(
        Point3D(0, 180, -100),
        RGBColor(1.0f, 0.95f, 0.8f),
        2.5f
    ));

    // Fill directional light to soften harsh shadows
    add_light(new DirectionalLight(
        Vector3D(1, -1, -1),
        RGBColor(0.15f, 0.15f, 0.2f)
    ));

    // =====================
    // MATERIALS
    // =====================
    Material* red_matte   = new Matte(RGBColor(0.85f, 0.08f, 0.08f), 0.75f, 0.1f);
    Material* green_matte = new Matte(RGBColor(0.08f, 0.75f, 0.08f), 0.75f, 0.1f);
    Material* white_matte = new Matte(RGBColor(0.88f, 0.88f, 0.88f), 0.7f,  0.1f);
    Material* blue_glossy = new Phong(RGBColor(0.1f, 0.2f, 0.85f), 0.6f, 0.4f, 80.0f, 0.08f);
    Material* mirror_mat  = new Mirror(0.9f, RGBColor(1.0f, 1.0f, 1.0f));
    Material* phong_white = new Phong(RGBColor(0.9f, 0.9f, 0.85f), 0.5f, 0.5f, 120.0f, 0.1f);

    // =====================
    // CORNELL BOX WALLS
    // =====================
    float sz = 200.0f;

    // Floor
    Plane* floor = new Plane(Point3D(0, -sz, 0), Vector3D(0, 1, 0));
    floor->set_material(white_matte);
    add_geometry(floor);

    // Ceiling
    Plane* ceiling = new Plane(Point3D(0, sz, 0), Vector3D(0, -1, 0));
    ceiling->set_material(white_matte);
    add_geometry(ceiling);

    // Back wall
    Plane* back = new Plane(Point3D(0, 0, -sz - 100), Vector3D(0, 0, 1));
    back->set_material(white_matte);
    add_geometry(back);

    // Left wall — red
    Plane* left = new Plane(Point3D(-sz, 0, 0), Vector3D(1, 0, 0));
    left->set_material(red_matte);
    add_geometry(left);

    // Right wall — green
    Plane* right = new Plane(Point3D(sz, 0, 0), Vector3D(-1, 0, 0));
    right->set_material(green_matte);
    add_geometry(right);

    // =====================
    // OBJECTS INSIDE BOX
    // =====================

    // Glossy blue sphere (left)
    Sphere* s1 = new Sphere(Point3D(-70, -130, -120), 65);
    s1->set_material(blue_glossy);
    add_geometry(s1);

    // Mirror sphere (right)
    Sphere* s2 = new Sphere(Point3D(80, -140, -170), 55);
    s2->set_material(mirror_mat);
    add_geometry(s2);

    // Small glossy white sphere on top of blue
    Sphere* s3 = new Sphere(Point3D(-70, 10, -80), 30);
    s3->set_material(phong_white);
    add_geometry(s3);

    // =====================
    // ACCELERATION STRUCTURE
    // =====================
    // Build BVH over all geometry added so far
    accel_ptr = new BVH(geometry);
    use_acceleration = true;
}
