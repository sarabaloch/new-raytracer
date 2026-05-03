/**
 * Cornell Box Scene — CS 440 Project 2
 * Reference: 4 spheres + orange triangle + rippled water inside Cornell box
 *
 * Box:  X∈[-260,260]  Y∈[-160,340]  Z∈[-360,160]
 * Front face open toward camera at (0,180,420).
 */

#include "../cameras/Perspective.hpp"
#include "../geometry/Sphere.hpp"
#include "../geometry/Triangle.hpp"
#include "../materials/Matte.hpp"
#include "../materials/Phong.hpp"
#include "../materials/Mirror.hpp"
#include "../samplers/Jittered.hpp"
#include "../light/PointLight.hpp"
#include "../light/DirectionalLight.hpp"
#include "../tracers/Shadow.hpp"
#include "../acceleration/BVH.hpp"
#include "../utilities/Constants.hpp"
#include "../world/World.hpp"
#include <cmath>
#include <cstdlib>
#include <ctime>

static void quad(World& w, Point3D a, Point3D b, Point3D c, Point3D d, Material* mat) {
    Triangle* t1 = new Triangle(a, b, c); t1->set_material(mat); w.add_geometry(t1);
    Triangle* t2 = new Triangle(a, c, d); t2->set_material(mat); w.add_geometry(t2);
}

void World::build(void) {

    // Seed rand so jitter varies per pixel rather than repeating the same pattern
    srand(42);

    // ── VIEW PLANE ────────────────────────────────────────────────────────
    vplane.top_left     = Point3D(-200,  150, 100);
    vplane.bottom_right = Point3D( 200, -150, 100);
    vplane.hres = 480;
    vplane.vres = 360;
    bg_color = RGBColor(0.01f, 0.01f, 0.01f);

    // ── CAMERA ───────────────────────────────────────────────────────────
    // Higher Y (180) and closer Z (420) so water surface fills lower half,
    // not the side-on edge of the mesh.
    set_camera(new Perspective(Point3D(0, 180, 420)));

    // ── SAMPLER  5×5 = 25 samples/pixel — removes grain ─────────────────
    // Fewer lights below compensate for the extra ray cost.
    sampler_ptr = new Jittered(camera_ptr, &vplane, 5);

    // ── TRACER ───────────────────────────────────────────────────────────
    tracer_ptr = new ShadowTracer(this);

    // ═════════════════════════════════════════════════════════════════════
    // MATERIALS
    // ka=0.30 ambient floor so nothing goes fully black in shadow.
    // ═════════════════════════════════════════════════════════════════════
    Material* white  = new Matte(RGBColor(0.76f, 0.76f, 0.76f), 0.80f, 0.30f);
    Material* red    = new Matte(RGBColor(0.65f, 0.05f, 0.05f), 0.80f, 0.30f);
    Material* green  = new Matte(RGBColor(0.12f, 0.48f, 0.08f), 0.80f, 0.30f);

    Material* water_mat  = new Mirror(0.90f, RGBColor(0.55f, 0.78f, 1.0f));
    Material* silver_mat = new Mirror(0.85f, RGBColor(0.88f, 0.90f, 0.95f));

    Material* pink_mat = new Phong(RGBColor(0.88f, 0.50f, 0.78f), 0.70f, 0.80f, 100.f, 0.30f);
    Material* dred_mat = new Phong(RGBColor(0.65f, 0.05f, 0.08f), 0.70f, 0.75f, 160.f, 0.30f);
    Material* teal_mat = new Phong(RGBColor(0.04f, 0.55f, 0.65f), 0.70f, 0.90f, 200.f, 0.30f);
    // Triangle: very high ka so its always visibly orange
    Material* tri_mat  = new Phong(RGBColor(0.96f, 0.44f, 0.05f), 0.85f, 0.55f,  40.f, 0.55f);

    // ═════════════════════════════════════════════════════════════════════
    // BOX WALLS   CCW winding as seen from INSIDE the box
    // ═════════════════════════════════════════════════════════════════════
    const float XL=-260,XR=260, YB=-160,YT=340, ZB=-360,ZF=160;

    // Floor
    quad(*this,{XL,YB,ZF},{XR,YB,ZF},{XR,YB,ZB},{XL,YB,ZB}, white);
    // Ceiling
    quad(*this,{XL,YT,ZB},{XR,YT,ZB},{XR,YT,ZF},{XL,YT,ZF}, white);
    // Back wall
    quad(*this,{XL,YB,ZB},{XL,YT,ZB},{XR,YT,ZB},{XR,YB,ZB}, white);
    // Left wall → red
    quad(*this,{XL,YB,ZF},{XL,YB,ZB},{XL,YT,ZB},{XL,YT,ZF}, red);
    // Right wall → green
    quad(*this,{XR,YB,ZB},{XR,YB,ZF},{XR,YT,ZF},{XR,YT,ZB}, green);

    // ═════════════════════════════════════════════════════════════════════
    // WATER RIPPLE MESH  100×100 quads, normals pointing UP
    // ═════════════════════════════════════════════════════════════════════
    {
        const float wy=-55.f, A=5.f, freq=0.050f;
        const int NX=100, NZ=100;
        const float x0=XL, x1=XR, z0=ZB, z1=ZF-30.f;
        const float dx=(x1-x0)/NX, dz=(z1-z0)/NZ;
        auto h=[&](float x,float z){ return wy+A*sinf(freq*sqrtf(x*x+z*z)); };
        for(int i=0;i<NX;i++) for(int j=0;j<NZ;j++){
            float xa=x0+i*dx, xb=xa+dx, za=z0+j*dz, zb=za+dz;
            Point3D p00(xa,h(xa,za),za), p10(xb,h(xb,za),za);
            Point3D p01(xa,h(xa,zb),zb), p11(xb,h(xb,zb),zb);
            // CCW from above → normal points up for correct reflections
            Triangle* t1=new Triangle(p00,p11,p10); t1->set_material(water_mat); add_geometry(t1);
            Triangle* t2=new Triangle(p00,p01,p11); t2->set_material(water_mat); add_geometry(t2);
        }
    }

    // ═════════════════════════════════════════════════════════════════════
    // ORANGE TRIANGLE — double-sided
    // ═════════════════════════════════════════════════════════════════════
    {
        Point3D apex(0,150,-110), bl(-110,-55,-110), br(110,-55,-110);
        Triangle* tf=new Triangle(bl,br,apex); tf->set_material(tri_mat); add_geometry(tf);
        Triangle* tb=new Triangle(br,bl,apex); tb->set_material(tri_mat); add_geometry(tb);
    }

    // ═════════════════════════════════════════════════════════════════════
    // SPHERES
    // ═════════════════════════════════════════════════════════════════════
    const float wy=-55.f;
    { float r=65; Sphere*s=new Sphere({-152,wy+r*.52f,-100},r); s->set_material(silver_mat); add_geometry(s); }
    { float r=55; Sphere*s=new Sphere({ -50,wy+r*.58f, -78},r); s->set_material(pink_mat);   add_geometry(s); }
    { float r=58; Sphere*s=new Sphere({  152,wy+r*.52f,-100},r); s->set_material(dred_mat);  add_geometry(s); }
    { float r=32; Sphere*s=new Sphere({    0,wy+r*.60f, -68},r); s->set_material(teal_mat);  add_geometry(s); }

    // ═════════════════════════════════════════════════════════════════════
    // LIGHTS — only 3 lights to keep shadow pass fast with 25 samples/px
    // ═════════════════════════════════════════════════════════════════════

    // 1. Main ceiling light — above centre, warm white
    add_light(new PointLight(
        Point3D(0, 320, -80),
        RGBColor(1.0f, 0.97f, 0.90f), 6.5f));

    // 2. Left fill — warm red tint, simulates red-wall colour bleeding
    add_light(new PointLight(
        Point3D(-250, 80, -80),
        RGBColor(1.0f, 0.72f, 0.72f), 4.0f));

    // 3. Right fill — green tint, simulates green-wall colour bleeding
    add_light(new PointLight(
        Point3D(250, 80, -80),
        RGBColor(0.72f, 1.0f, 0.72f), 4.0f));

    // ═════════════════════════════════════════════════════════════════════
    // BVH  (set use_acceleration=false to benchmark without)
    // ═════════════════════════════════════════════════════════════════════
    accel_ptr        = new BVH(geometry);
    use_acceleration = true;
}
