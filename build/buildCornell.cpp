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

    bg_color = RGBColor(0.02f, 0.01f, 0.05f);  // Deep cyberpunk purple-black

    // =====================
    // CAMERA
    // =====================
    set_camera(new Perspective(Point3D(0, 0, 550)));

    // =====================
    // SAMPLER (higher samples for cleaner neon in dark scenes)
    // =====================
    sampler_ptr = new Jittered(camera_ptr, &vplane, 4);

    // =====================
    // TRACERS
    // =====================
    tracer_ptr = new ShadowTracer(this);

    // =====================
    // LIGHTS - CYBERPUNK THEME (dark with neon accents)
    // =====================
    
    // Dim ambient-like ceiling light (much darker)
    add_light(new PointLight(
        Point3D(0, 160, -100),
        RGBColor(0.15f, 0.12f, 0.25f),  // Very dim purple-white
        0.8f  // Low intensity
    ));
    
    // Cyberpunk pink/magenta light from right side
    add_light(new PointLight(
        Point3D(180, 60, -150),
        RGBColor(1.8f, 0.2f, 1.2f),  // Intense magenta
        2.2f
    ));
    
    // Cyan light from left side
    add_light(new PointLight(
        Point3D(-180, 40, -140),
        RGBColor(0.1f, 1.2f, 1.5f),  // Bright cyan
        2.0f
    ));
    
    // Dim orange/yellow light from below (industrial cyberpunk)
    add_light(new PointLight(
        Point3D(0, -160, -120),
        RGBColor(1.0f, 0.4f, 0.1f),  // Amber/orange
        1.2f
    ));
    
    // Back rim light - deep blue/purple
    add_light(new PointLight(
        Point3D(0, 30, -290),
        RGBColor(0.3f, 0.1f, 1.2f),  // Deep purple/blue
        1.0f
    ));
    
    // Very subtle fill from directional light (almost dark)
    add_light(new DirectionalLight(
        Vector3D(0.3f, -0.6f, -0.7f),
        RGBColor(0.05f, 0.04f, 0.08f)  // Very dark purple-gray
    ));

    // =====================
    // MATERIALS - CYBERPUNK DARK THEME
    // =====================
    // Darker, more saturated materials
    Material* red_matte   = new Matte(RGBColor(0.7f, 0.08f, 0.12f), 0.7f, 0.08f);   // Deep blood red
    Material* green_matte = new Matte(RGBColor(0.05f, 0.6f, 0.15f), 0.7f, 0.08f);   // Dark neon green
    Material* white_matte = new Matte(RGBColor(0.35f, 0.35f, 0.45f), 0.65f, 0.05f); // Dark gray with purple tint
    Material* blue_glossy = new Phong(RGBColor(0.08f, 0.15f, 0.7f), 0.6f, 0.5f, 120.0f, 0.05f);  // Dark glossy blue
    Material* mirror_mat  = new Mirror(0.88f, RGBColor(0.95f, 0.9f, 1.0f));  // Slightly purple-tinted mirror
    Material* phong_white = new Phong(RGBColor(0.7f, 0.65f, 0.8f), 0.4f, 0.6f, 150.0f, 0.05f);  // Iridescent white
    
    // Cyberpunk metal materials
    Material* dark_metal = new Phong(RGBColor(0.25f, 0.25f, 0.35f), 0.4f, 0.7f, 200.0f, 0.02f);

    // =====================
    // CORNELL BOX WALLS - Dark cyberpunk versions
    // =====================
    float sz = 200.0f;

    // Floor - dark metallic with slight reflection
    Plane* floor = new Plane(Point3D(0, -sz, 0), Vector3D(0, 1, 0));
    floor->set_material(dark_metal);
    add_geometry(floor);

    // Ceiling - very dark, almost black
    Material* ceiling_mat = new Matte(RGBColor(0.12f, 0.10f, 0.15f), 0.5f, 0.02f);
    Plane* ceiling = new Plane(Point3D(0, sz, 0), Vector3D(0, -1, 0));
    ceiling->set_material(ceiling_mat);
    add_geometry(ceiling);

    // Back wall - dark with slight purple tint
    Material* back_wall = new Matte(RGBColor(0.2f, 0.18f, 0.28f), 0.6f, 0.05f);
    Plane* back = new Plane(Point3D(0, 0, -sz - 100), Vector3D(0, 0, 1));
    back->set_material(back_wall);
    add_geometry(back);

    // Left wall — dark blood red
    Plane* left = new Plane(Point3D(-sz, 0, 0), Vector3D(1, 0, 0));
    left->set_material(red_matte);
    add_geometry(left);

    // Right wall — dark toxic green
    Plane* right = new Plane(Point3D(sz, 0, 0), Vector3D(-1, 0, 0));
    right->set_material(green_matte);
    add_geometry(right);

    // =====================
    // OBJECTS INSIDE BOX (unchanged positions, darker materials)
    // =====================

    // Dark glossy blue sphere (left)
    Sphere* s1 = new Sphere(Point3D(-70, -130, -120), 65);
    s1->set_material(blue_glossy);
    add_geometry(s1);

    // Mirror sphere (right) - will reflect the neon cyberpunk lights
    Sphere* s2 = new Sphere(Point3D(80, -140, -170), 55);
    s2->set_material(mirror_mat);
    add_geometry(s2);

    // Small metallic/iridescent sphere on top of blue
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
