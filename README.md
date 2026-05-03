# CS 440 Assignment 5 — Cornell Box Ray Tracer

## Building

```bash
g++ -std=c++17 -O2 -I. \
  raytracer.cpp \
  acceleration/BVH.cpp \
  brdf/Lambertian.cpp brdf/GlossySpecular.cpp brdf/PerfectSpecular.cpp \
  build/buildCornell.cpp \
  cameras/Parallel.cpp cameras/Perspective.cpp \
  geometry/Geometry.cpp geometry/Plane.cpp geometry/Sphere.cpp geometry/Traingle.cpp \
  image/Image.cpp \
  light/DirectionalLight.cpp light/PointLight.cpp \
  materials/Cosine.cpp materials/Matte.cpp materials/Mirror.cpp materials/Phong.cpp \
  samplers/Sampler.cpp samplers/Simple.cpp samplers/Jittered.cpp \
  tracers/Tracer.cpp tracers/Basic.cpp tracers/Shadow.cpp \
  utilities/BBox.cpp utilities/Point3d.cpp utilities/Ray.cpp utilities/RGBColor.cpp \
  utilities/ShadeInfo.cpp utilities/Vector3D.cpp \
  world/ViewPlane.cpp world/World.cpp \
  -o raytracer_cornell

./raytracer_cornell
```

Output is written to `scene.png`.

---

## Features Implemented

### Appearance — `brdf/` folder

Three BRDF classes implement distinct reflection models:

| Class | File | Material |
|---|---|---|
| `Lambertian` | `brdf/Lambertian.hpp/cpp` | Diffuse: `f = kd * cd / π` |
| `GlossySpecular` | `brdf/GlossySpecular.hpp/cpp` | Phong glossy: `f = ks * cs * (r·wo)^exp` |
| `PerfectSpecular` | `brdf/PerfectSpecular.hpp/cpp` | Mirror reflection via `sample_f` |

Three `Material` subclasses use these BRDFs:

- **`Matte`** — uses `Lambertian` for ambient + diffuse shading
- **`Phong`** — uses `Lambertian` + `GlossySpecular` for diffuse + specular highlights
- **`Mirror`** — uses `PerfectSpecular` for recursive mirror reflections

---

### Lighting — `light/` folder

Two light types are implemented under a `Light` base class:

- **`PointLight`** — light at a position, with color and intensity. Used for the main ceiling light.
- **`DirectionalLight`** — infinite directional light. Used as a fill light to soften shadows.

Both are stored in `World::lights` (a `std::vector<Light*>`).

---

### Acceleration — `acceleration/` folder

A **BVH (Bounding Volume Hierarchy)** is implemented in `acceleration/BVH.hpp/cpp`.

**To enable/disable the acceleration structure:**

In `build/buildCornell.cpp`, the last lines of `World::build()` are:

```cpp
accel_ptr = new BVH(geometry);
use_acceleration = true;   // ← set to false to disable BVH
```

Set `use_acceleration = false` (and comment out the `BVH` construction) to render using brute-force ray-object intersection. In `world/World.cpp`, `hit_objects()` checks this flag and dispatches accordingly.

---

### Ray Casting — `tracers/` folder

A `Tracer` hierarchy is used for all shading:

- **`BasicTracer`** — shades with primary rays only (no shadows)
- **`ShadowTracer`** — shoots shadow rays toward each light before contributing direct illumination; objects in shadow receive only ambient light

`World::tracer_ptr` holds the active tracer. `raytracer.cpp` calls `tracer->trace_ray(ray, depth)` per sample.

To switch tracers, change `buildCornell.cpp`:
```cpp
tracer_ptr = new ShadowTracer(this);   // with shadows
// tracer_ptr = new BasicTracer(this); // without shadows
```

---

### Sampling — `samplers/` folder

In addition to `Simple` (one ray per pixel), a **`Jittered`** sampler implements stratified anti-aliasing:

- Each pixel is divided into an `n × n` grid of sub-cells.
- One ray is shot through a random point within each sub-cell.
- All ray weights sum to 1 (`w = 1 / n²`).
- The Cornell box uses `n = 3` (9 samples per pixel) to reduce jagged edges.

To switch samplers in `buildCornell.cpp`:
```cpp
sampler_ptr = new Jittered(camera_ptr, &vplane, 3); // 3x3 jittered
// sampler_ptr = new Simple(camera_ptr, &vplane);   // 1 sample per pixel
```

---

## Cornell Box Scene

The scene uses a `Perspective` camera at `(0, 0, 550)` looking into a 400-unit box:

| Wall | Color | Material |
|---|---|---|
| Floor | White | Matte |
| Ceiling | White | Matte |
| Back wall | White | Matte |
| Left wall | Red | Matte |
| Right wall | Green | Matte |

Three spheres inside the box:
- **Blue glossy sphere** (left) — Phong material with specular highlights
- **Mirror sphere** (right) — Perfect specular reflections showing the red/green walls
- **Small white sphere** (top) — Phong material

Two lights:
- Point light at `(0, 180, -100)` — warm white ceiling light
- Directional fill light — cool-tinted from upper-right, softens shadow edges

---

## Project 2 Additions

### New Material — `materials/PoolWater`

A semi-transparent water material that blends a perfect specular reflection with a straight-through transmitted ray, revealing pool tiles below the surface.

```
PoolWater(kr, reflect_weight, tint)
  kr             — specular reflectance
  reflect_weight — 0=fully transparent, 1=fully mirror (0.78 used in scene)
  tint           — colour applied to the transmitted ray
```

### Scene — Cornell Box Pool (`build/buildCornell.cpp`)

A Cornell box containing a pool of water with:
- **Pool floor + walls**: checkerboard cyan tiles with white grout (T=50 units, G=3 grout gap)
- **Water**: `PoolWater` semi-transparent mirror surface with sine-wave ripple mesh
- **Water mesh**: 710×710 grid = **1,008,200 triangles** (meets 1M primitive requirement)
- **Pyramid**: 4-faced sandstone pyramid with distinct face shading for 3D appearance
- **Spheres**: silver-grey Phong, ruby red, emerald green
- **Lighting**: warm golden key (PointLight) + cool blue fills (PointLight) + warm directional ambient (DirectionalLight)
- **Acceleration**: BVH (`use_acceleration = true`; set to `false` to benchmark without)
- **Sampler**: Jittered 4×4 = 16 samples/pixel for anti-aliasing
- **Tracer**: ShadowTracer with `ambient_floor = 0.80`

### Compile Command

```bash
g++ -std=c++17 -O2 -I. \
  raytracer.cpp \
  acceleration/BVH.cpp \
  brdf/Lambertian.cpp brdf/GlossySpecular.cpp brdf/PerfectSpecular.cpp \
  build/buildCornell.cpp \
  cameras/Parallel.cpp cameras/Perspective.cpp \
  geometry/Geometry.cpp geometry/Plane.cpp geometry/Sphere.cpp geometry/Traingle.cpp \
  image/Image.cpp \
  light/DirectionalLight.cpp light/PointLight.cpp \
  materials/Cosine.cpp materials/Matte.cpp materials/Mirror.cpp materials/Phong.cpp \
  materials/PoolWater.cpp \
  samplers/Sampler.cpp samplers/Simple.cpp samplers/Jittered.cpp \
  tracers/Tracer.cpp tracers/Basic.cpp tracers/Shadow.cpp \
  utilities/BBox.cpp utilities/Point3d.cpp utilities/Ray.cpp utilities/RGBColor.cpp \
  utilities/ShadeInfo.cpp utilities/Vector3D.cpp \
  world/ViewPlane.cpp world/World.cpp \
  -o raytracer_cornell

./raytracer_cornell
```

Output written to `scene.png`. Toggle `use_acceleration` in `buildCornell.cpp` to compare BVH vs brute-force performance.

### Area Light (`light/AreaLight`)

A rectangular emitter that produces **soft shadows with penumbras**.

```cpp
AreaLight(corner, u, v, color, intensity, num_samples)
```

- `corner` — one corner of the rectangle
- `u`, `v` — two edge vectors defining the rectangle's size and orientation
- `num_samples` — shadow rays cast per shading point (16 used in scene)

Each shadow ray targets a **different random point** on the rectangle. The fraction of unoccluded rays determines how lit the surface is — producing sharp shadows near contact points and soft blurry penumbras further away.

The `ShadowTracer` detects area lights via `is_area_light()` and uses the multi-sample path automatically. Point/directional lights continue to use single hard shadow rays.
