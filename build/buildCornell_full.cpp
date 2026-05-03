#include "../cameras/Perspective.hpp"
#include "../geometry/Plane.hpp"
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

static void quad(World& w, Point3D a, Point3D b, Point3D c, Point3D d, Material* mat) {
    Triangle* t1 = new Triangle(a, b, c); t1->set_material(mat); w.add_geometry(t1);
    Triangle* t2 = new Triangle(a, c, d); t2->set_material(mat); w.add_geometry(t2);
}

void World::build(void) {
    srand(42);

    vplane.top_left     = Point3D(-240,  220, 100);
    vplane.bottom_right = Point3D( 240, -140, 100);
    vplane.hres = 480;
    vplane.vres = 360;
    bg_color = RGBColor(0.0f, 0.0f, 0.0f);

    // Camera at (0,280,650) — elevated, looking down into box
    set_camera(new Perspective(Point3D(0, 280, 650)));

    // 3x3 for fast test iteration
    sampler_ptr = new Jittered(camera_ptr, &vplane, 3);
    tracer_ptr  = new ShadowTracer(this);

    // ── MATERIALS (from uploaded version) ───────────────────────────────
    Material* white_mat      = new Matte(RGBColor(0.88f, 0.82f, 0.62f), 0.90f, 0.30f);
    Material* left_wall_mat  = new Matte(RGBColor(0.28f, 0.08f, 0.48f), 0.90f, 0.25f);
    Material* right_wall_mat = new Matte(RGBColor(0.05f, 0.12f, 0.55f), 0.90f, 0.25f);
    Material* water_mat      = new Mirror(0.92f, RGBColor(0.25f, 0.55f, 1.0f));
    Material* silver_mat     = new Mirror(0.88f, RGBColor(0.80f, 0.88f, 1.0f));
    Material* pink_mat       = new Phong(RGBColor(0.62f, 0.45f, 0.90f), 0.70f, 0.85f, 150.0f, 0.35f);
    Material* red_mat        = new Phong(RGBColor(0.95f, 0.72f, 0.08f), 0.70f, 0.75f, 200.0f, 0.30f);
    Material* teal_mat       = new Phong(RGBColor(0.05f, 0.75f, 0.85f), 0.70f, 0.95f, 300.0f, 0.35f);
    Material* tri_mat        = new Phong(RGBColor(0.92f, 0.78f, 0.10f), 0.80f, 0.55f,  80.0f, 0.50f);

    // ── BOX ─────────────────────────────────────────────────────────────
    const float BX0=-300,BX1=300, BY0=-200,BY1=400, BZ0=-400;
    const float ZF=700; // front corners extend well past camera

    // Floor
    quad(*this, {BX0,BY0,BZ0},{BX1,BY0,BZ0},{BX1,BY0,ZF},{BX0,BY0,ZF}, white_mat);
    // Ceiling
    quad(*this, {BX0,BY1,ZF},{BX1,BY1,ZF},{BX1,BY1,BZ0},{BX0,BY1,BZ0}, white_mat);
    // Back wall
    quad(*this, {BX0,BY0,BZ0},{BX0,BY1,BZ0},{BX1,BY1,BZ0},{BX1,BY0,BZ0}, white_mat);
    // Left wall (red/purple)
    quad(*this, {BX0,BY0,ZF},{BX0,BY0,BZ0},{BX0,BY1,BZ0},{BX0,BY1,ZF}, left_wall_mat);
    // Right wall (green/blue)
    quad(*this, {BX1,BY0,BZ0},{BX1,BY0,ZF},{BX1,BY1,ZF},{BX1,BY1,BZ0}, right_wall_mat);

    // ── PARTIAL FRONT WALL ("baseboard") ────────────────────────────────
    // Blocks the camera from seeing the near edge of the water mesh.
    // Calculated: camera(0,280,650) bottom pixel hits water at z≈211.
    // A wall at z=300, from floor(BY0=-200) up to y=25, covers the gap.
    // Its top edge appears at vplane y≈-129, i.e. only the last 3% of frame height.
    const float WALL_Z   = 300.0f;
    const float WALL_TOP =  25.0f;   // just above water_y=-55... actually above the ripling peak
    quad(*this,
         {BX0, BY0,  WALL_Z}, {BX1, BY0,  WALL_Z},
         {BX1, WALL_TOP, WALL_Z}, {BX0, WALL_TOP, WALL_Z},
         white_mat);

    // ── WATER MESH ───────────────────────────────────────────────────────
    // Extends from back wall to front wall (z=300), normals UP
    {
        const float wy=-55.f, A=5.f, freq=0.055f;
        const int NX=100, NZ=100;
        // Water only behind the front wall
        const float x0=BX0,x1=BX1, z0=BZ0,z1=WALL_Z;
        const float dx=(x1-x0)/NX, dz=(z1-z0)/NZ;
        auto h=[&](float x,float z){ return wy+A*sinf(freq*sqrtf(x*x+z*z)); };
        for(int i=0;i<NX;i++) for(int j=0;j<NZ;j++){
            float xa=x0+i*dx,xb=xa+dx,za=z0+j*dz,zb=za+dz;
            Point3D p00(xa,h(xa,za),za),p10(xb,h(xb,za),za);
            Point3D p01(xa,h(xa,zb),zb),p11(xb,h(xb,zb),zb);
            Triangle*t1=new Triangle(p00,p11,p10); t1->set_material(water_mat); add_geometry(t1);
            Triangle*t2=new Triangle(p00,p01,p11); t2->set_material(water_mat); add_geometry(t2);
        }
    }

    // ── TRIANGLE ────────────────────────────────────────────────────────
    {
        Point3D apex(0,140,-100), bl(-120,-55,-100), br(120,-55,-100);
        Triangle*tf=new Triangle(bl,br,apex); tf->set_material(tri_mat); add_geometry(tf);
        Triangle*tb=new Triangle(br,bl,apex); tb->set_material(tri_mat); add_geometry(tb);
    }

    // ── SPHERES ─────────────────────────────────────────────────────────
    const float wy=-55.f;
    { float r=70; Sphere*s=new Sphere({-160,wy+r*.55f,-90},r); s->set_material(silver_mat); add_geometry(s); }
    { float r=58; Sphere*s=new Sphere({ -55,wy+r*.60f,-70},r); s->set_material(pink_mat);   add_geometry(s); }
    { float r=62; Sphere*s=new Sphere({  160,wy+r*.55f,-90},r); s->set_material(red_mat);   add_geometry(s); }
    { float r=36; Sphere*s=new Sphere({    0,wy+r*.65f,-60},r); s->set_material(teal_mat);  add_geometry(s); }

    // ── LIGHTS ──────────────────────────────────────────────────────────
    add_light(new PointLight({  0, 370,-100}, {1.0f,0.88f,0.45f}, 9.0f));
    add_light(new PointLight({  0, 360,  80}, {1.0f,0.82f,0.30f}, 6.5f));
    add_light(new PointLight({-280, 150,-100}, {0.60f,0.30f,1.0f}, 5.0f));
    add_light(new PointLight({ 280, 150,-100}, {0.25f,0.50f,1.0f}, 5.0f));
    add_light(new PointLight({  0,  100,-380}, {0.45f,0.35f,1.0f}, 4.5f));
    add_light(new DirectionalLight({0,-1,-0.1f}, {0.55f,0.50f,0.40f}));

    accel_ptr        = new BVH(geometry);
    use_acceleration = true;
}
