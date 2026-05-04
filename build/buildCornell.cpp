#include "../cameras/Perspective.hpp"
#include "../geometry/Plane.hpp"
#include "../geometry/Sphere.hpp"
#include "../geometry/Triangle.hpp"
#include "../materials/Matte.hpp"
#include "../materials/Phong.hpp"
#include "../materials/Mirror.hpp"
#include "../materials/PoolWater.hpp"
#include "../samplers/Jittered.hpp"
#include "../light/PointLight.hpp"
#include "../light/DirectionalLight.hpp"
#include "../light/AreaLight.hpp"
#include "../tracers/Shadow.hpp"
#include "../acceleration/BVH.hpp"
#include "../utilities/Constants.hpp"
#include "../world/World.hpp"
#include <cmath>
#include <cstdlib>

//helper function for creating a rectangle using two triangles
static void quad(World& w, Point3D a, Point3D b, Point3D c, Point3D d, Material* mat) {
    Triangle* t1 = new Triangle(a, b, c); 
    t1->set_material(mat); 
    w.add_geometry(t1);

    Triangle* t2 = new Triangle(a, c, d); 
    t2->set_material(mat); 
    w.add_geometry(t2);
}

//double sided quad 
static void dquad(World& w, Point3D a, Point3D b, Point3D c, Point3D d, Material* mat) {
    quad(w,a,b,c,d,mat); quad(w,d,c,b,a,mat);
}

void World::build(void) {
    srand(42); //fixed randomness

    //16:9 aspect ratio
    //black background
    vplane.top_left     = Point3D(-240,  160, 100);  
    vplane.bottom_right = Point3D( 240, -200, 100);  
    vplane.hres = 480;
    vplane.vres = 360;
    bg_color = RGBColor(0.0f, 0.0f, 0.0f);

    set_camera(new Perspective(Point3D(0, 80, 650)));

    //4 samples per pixel therefore smoother
    sampler_ptr = new Jittered(camera_ptr, &vplane, 4);
    tracer_ptr  = new ShadowTracer(this);

    // materials: mirror, phong, poolwater, matte
    // colours from Code 2 (buildCornellHQ)
    Material* floor_mat  = new Matte(RGBColor(0.95f, 0.88f, 0.72f), 0.80f, 0.55f);
    Material* ceil_mat   = new Matte(RGBColor(0.10f, 0.22f, 0.85f), 0.80f, 0.52f);
    Material* back_mirror_mat = new Mirror(0.92f, RGBColor(0.88f, 0.88f, 0.92f));
    Material* back_mat   = new Matte(RGBColor(0.40f, 0.45f, 0.80f), 0.80f, 0.55f);
    Material* left_mat   = new Matte(RGBColor(0.35f, 0.22f, 0.42f), 0.80f, 0.50f);
    Material* right_mat  = new Matte(RGBColor(0.75f, 0.30f, 0.65f), 0.80f, 0.55f);

    Material* tile_col  = new Matte(RGBColor(0.18f, 0.22f, 0.45f), 0.80f, 0.48f);
    Material* grout_mat = new Matte(RGBColor(0.96f, 0.98f, 0.98f), 0.70f, 0.85f);

    // water with refraction
    Material* water_mat  = new PoolWater(0.92f, 0.08f, RGBColor(0.10f, 0.14f, 0.38f));
    
    // reflective sphere
    Material* silver_mat  = new Mirror(0.95f, RGBColor(0.85f, 0.87f, 0.90f));
    
    Material* pink_mat   = new Phong(RGBColor(0.85f, 0.08f, 0.12f), 0.65f, 0.90f, 200.f, 0.40f);
    Material* amber_mat  = new Phong(RGBColor(0.08f, 0.62f, 0.28f), 0.65f, 0.90f, 250.f, 0.40f);
    Material* orange_mat = new Phong(RGBColor(0.95f, 0.45f, 0.05f), 0.65f, 0.88f, 220.f, 0.40f);
    Material* teal_mat   = new Phong(RGBColor(0.92f, 0.72f, 0.08f), 0.65f, 0.95f, 300.f, 0.40f);

    Material* pyr_mat   = new Phong(RGBColor(1.00f, 0.88f, 0.55f), 0.80f, 0.50f, 60.f, 0.60f);
    Material* pyr_dark  = new Phong(RGBColor(0.85f, 0.68f, 0.35f), 0.78f, 0.40f, 40.f, 0.55f);

    // left, right, bottom, top bounds for pool
    const float BX0 =- 400, BX1 = 400, BY0 =- 200, BY1 = 200, BZ0 =- 400;
    const float WALL_Z = 800.f;
    const float WATER_Y =- 55.f;
    const float T = 50.f;
    const float G = 3.f;

    //ceiling
    dquad(*this,{BX0,BY1,BZ0},{BX1,BY1,BZ0},{BX1,BY1,WALL_Z},{BX0,BY1,WALL_Z}, ceil_mat);

    //pool floor
    dquad(*this,{BX0,BY0,BZ0},{BX1,BY0,BZ0},{BX1,BY0,1200.f},{BX0,BY0,1200.f}, grout_mat);

    for(int ix = 0; ix<(int)((BX1-BX0)/T); ix++)
        for(int iz=0; iz<(int)((WALL_Z-BZ0)/T); iz++){
            float xa = BX0+ix*T+G,  xb = BX0+(ix+1)*T-G;
            float za = BZ0+iz*T+G,  zb = BZ0+(iz+1)*T-G;
            quad(*this,{xa,BY0+0.5f,za},{xb,BY0+0.5f,za},{xb,BY0+0.5f,zb},{xa,BY0+0.5f,zb}, tile_col);
        }

    // pool walls

    //left
    dquad(*this,{BX0,BY0,BZ0},{BX0,BY0,WALL_Z},{BX0,WATER_Y-6.f,WALL_Z},{BX0,WATER_Y-6.f,BZ0}, grout_mat);
    for(int iz=0; iz<(int)((WALL_Z-BZ0)/T); iz++)
        for(int iy=0; iy<(int)((WATER_Y-BY0)/T); iy++){
            float za = BZ0+iz*T+G,  zb = BZ0+(iz+1)*T-G;
            float ya = BY0+iy*T+G,  yb = BY0+(iy+1)*T-G;
            if(yb > WATER_Y-6.f-G) {
                yb = WATER_Y-6.f-G;
            }
            quad(*this, {BX0+0.5f,ya,za},{BX0+0.5f,ya,zb}, {BX0+0.5f,yb,zb}, {BX0+0.5f,yb,za}, tile_col);
        }
    quad(*this, {BX0,WATER_Y-6.f,WALL_Z}, {BX0,WATER_Y-6.f,BZ0}, {BX0,BY1,BZ0}, {BX0,BY1,WALL_Z}, left_mat);

    //right
    dquad(*this, {BX1,BY0,WALL_Z}, {BX1,BY0,BZ0}, {BX1,WATER_Y-6.f,BZ0}, {BX1,WATER_Y-6.f,WALL_Z}, grout_mat);
    for(int iz=0; iz<(int)((WALL_Z-BZ0)/T); iz++)
        for(int iy=0; iy<(int)((WATER_Y-BY0)/T); iy++){
            float za = BZ0+iz*T+G,  zb = BZ0+(iz+1)*T-G;
            float ya = BY0+iy*T+G,  yb = BY0+(iy+1)*T-G;
            if(yb > WATER_Y-6.f-G) {
                yb=WATER_Y-6.f-G;
            }
            quad(*this, {BX1-0.5f,ya,zb}, {BX1-0.5f,ya,za}, {BX1-0.5f,yb,za}, {BX1-0.5f,yb,zb}, tile_col);
        }
    quad(*this, {BX1,WATER_Y-6.f,BZ0}, {BX1,WATER_Y-6.f,WALL_Z}, {BX1,BY1,WALL_Z}, {BX1,BY1,BZ0}, right_mat);

    //back
    dquad(*this, {BX0,BY0,BZ0}, {BX0,WATER_Y-6.f,BZ0}, {BX1,WATER_Y-6.f,BZ0}, {BX1,BY0,BZ0}, grout_mat);
    for(int ix=0; ix<(int)((BX1-BX0)/T); ix++)
        for(int iy=0; iy<(int)((WATER_Y-BY0)/T); iy++){
            float xa = BX0+ix*T+G,  xb = BX0+(ix+1)*T-G;
            float ya = BY0+iy*T+G,  yb = BY0+(iy+1)*T-G;
            if(yb>WATER_Y-6.f-G){
                yb=WATER_Y-6.f-G;
            }
            quad(*this, {xa,ya,BZ0+0.5f}, {xa,yb,BZ0+0.5f}, {xb,yb,BZ0+0.5f}, {xb,ya,BZ0+0.5f}, tile_col);
        }
    quad(*this, {BX0,WATER_Y-6.f,BZ0}, {BX0,BY1,BZ0}, {BX1,BY1,BZ0}, {BX1,WATER_Y-6.f,BZ0}, back_mat);

    //water
    {
        const float wy = WATER_Y;
        
        //water height as a function
        auto height = [&](float x, float z) {
            float y = wy;
            
            //large smooth wave
            y += 0.6f * sinf(x * 0.015f) * cosf(z * 0.015f);
            y += 0.3f * sinf(x * 0.06f + 1.2f) * sinf(z * 0.05f);
            
            auto sphereRipple = [&](float x, float z, float cx, float cz, float strength, float width) {
                float dx = x - cx;
                float dz = z - cz;
                float dist = sqrtf(dx*dx + dz*dz);
                if (dist < width && dist > 0.1f) {
                    float t = 1.0f - (dist / width);
                    float falloff = t * t * (3.0f - 2.0f * t);
                    return strength * falloff * cosf(dist * 0.35f);
                }
                return 0.0f;
            };
            
            // Micro-ripples
            y += 0.2f * sinf(x * 0.12f + z * 0.10f);
            y += 0.15f * sinf(x * 0.3f - z * 0.25f);
            
            return y;
        };
        
        const float WATER_Z1 = 1200.f;
        const int NX = 707, NZ = 707;
        const float dx = (BX1 - BX0) / NX;
        const float dz = (WATER_Z1 - BZ0) / NZ;
        
        for(int i = 0; i < NX; i++) {
            for(int j = 0; j < NZ; j++) {
                float xa = BX0 + i * dx, xb = xa + dx;
                float za = BZ0 + j * dz, zb = za + dz;
                
                Point3D p00(xa, height(xa, za), za);
                Point3D p10(xb, height(xb, za), za);
                Point3D p01(xa, height(xa, zb), zb);
                Point3D p11(xb, height(xb, zb), zb);
                
                Triangle* t1 = new Triangle(p00, p11, p10);
                t1->set_material(water_mat);
                add_geometry(t1);
                
                Triangle* t2 = new Triangle(p00, p01, p11);
                t2->set_material(water_mat);
                add_geometry(t2);
            }
        }
    }

    //pyramid
    {
        const float py = WATER_Y;
        const float S  = 80.f; 
        const float pz = -220.f;
        const float apex_y = 85.f;
        const float apex_z = pz;

        Point3D A( 0,   apex_y, apex_z);
        Point3D FL(-S,  py,     pz-S);
        Point3D FR( S,  py,     pz-S);
        Point3D BL(-S,  py,     pz+S);
        Point3D BR( S,  py,     pz+S);

        Triangle* f1 = new Triangle(FL, FR, A);
        f1->set_material(pyr_mat);
        add_geometry(f1);
        
        Triangle* f2 = new Triangle(BR, BL, A);
        f2->set_material(pyr_dark);
        add_geometry(f2);
        
        Triangle* f3 = new Triangle(BL, FL, A);
        f3->set_material(pyr_dark);
        add_geometry(f3);
        
        Triangle* f4 = new Triangle(FR, BR, A);
        f4->set_material(pyr_mat);
        add_geometry(f4);
        
        dquad(*this, FL, FR, BR, BL, pyr_dark);
    }

    // spheres
    const float wy = WATER_Y;
    
    // reflective
    { 
        float r = 75.f;
        float sphere_y = wy + r * 0.28f;
        Sphere* s = new Sphere({-280, sphere_y, -100}, r);
        s->set_material(silver_mat);
        add_geometry(s);
    }
    
    // red
    { 
        float r = 65.f;
        float sphere_y = wy + r * 0.35f;
        Sphere* s = new Sphere({-80, sphere_y, -80}, r);
        s->set_material(pink_mat);
        add_geometry(s);
    }
    
    // green
    { 
        float r = 70.f;
        Sphere* s = new Sphere({220, wy + r * 0.58f, -100}, r);
        s->set_material(amber_mat);
        add_geometry(s);
    }

    // orange + ripples
    {
        float r = 42.f;
        float cx = 55.f, cz = -80.f;
        float sphere_y = WATER_Y + r * 0.48f;
        Sphere* s = new Sphere({cx, sphere_y, cz}, r);
        s->set_material(orange_mat);
        add_geometry(s);

        int   segs = 64;
        float pi2 = 6.28318f;

        struct Ring { float inner_r; float outer_r; float peak_height; };
        Ring rings[] = {
            {52.f,  68.f,  7.0f},
            {78.f,  90.f,  5.2f},
            {100.f, 112.f, 3.5f},
            {122.f, 133.f, 2.2f},
            {142.f, 152.f, 1.3f},
        };

        for (auto& ring : rings) {
            float ri = ring.inner_r, ro = ring.outer_r;
            float base_height = ring.peak_height;
            
            for (int i = 0; i < segs; i++) {
                float a0 = (i    ) * pi2 / segs;
                float a1 = (i + 1) * pi2 / segs;
                
                float h_inner0 = WATER_Y + base_height * (0.7f + 0.3f * cosf(a0 * 3.0f));
                float h_outer0 = WATER_Y + base_height * 0.15f * (1.0f + sinf(a0 * 4.0f));
                float h_inner1 = WATER_Y + base_height * (0.7f + 0.3f * cosf(a1 * 3.0f));
                float h_outer1 = WATER_Y + base_height * 0.15f * (1.0f + sinf(a1 * 4.0f));
                
                Point3D p00(cx + ri * cosf(a0), h_inner0, cz + ri * sinf(a0));
                Point3D p10(cx + ro * cosf(a0), h_outer0, cz + ro * sinf(a0));
                Point3D p01(cx + ri * cosf(a1), h_inner1, cz + ri * sinf(a1));
                Point3D p11(cx + ro * cosf(a1), h_outer1, cz + ro * sinf(a1));
                
                Triangle* t1 = new Triangle(p00, p11, p10);
                t1->set_material(water_mat);
                add_geometry(t1);
                
                Triangle* t2 = new Triangle(p00, p01, p11);
                t2->set_material(water_mat);
                add_geometry(t2);
            }
        }
    }

    // lights
    add_light(new AreaLight(
        Point3D(-200.f, 260.f, -180.f),
        Vector3D(400.f,   0.f,   0.f),
        Vector3D(  0.f,   0.f, 200.f),
        RGBColor(1.00f, 0.90f, 0.60f),
        14.0f,
        16
    ));
    add_light(new PointLight({-285, 180, -100}, {0.35f, 0.50f, 1.00f},  7.0f));
    add_light(new PointLight({ 285, 180, -100}, {0.45f, 0.35f, 0.90f},  7.0f));
    add_light(new PointLight({-280, WATER_Y - 35, -100}, {0.45f, 0.60f, 0.85f},  9.0f));
    add_light(new PointLight({60, WATER_Y - 25, -80}, {0.50f, 0.55f, 0.75f},  7.0f));
    add_light(new DirectionalLight({0.f, -1.f, -0.05f}, {0.90f, 0.85f, 0.70f}));

    accel_ptr        = new BVH(geometry);
    use_acceleration = true;
}