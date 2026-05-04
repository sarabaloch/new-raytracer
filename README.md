# CS 440 Project 2 — Cornell Box Pool Ray Tracer

## Building & Running

### Low Quality Render (480×360) — `buildCornell.cpp`

```bash
g++ -std=c++17 -O2 -I. \
  raytracer.cpp \
  acceleration/BVH.cpp \
  brdf/Lambertian.cpp brdf/GlossySpecular.cpp brdf/PerfectSpecular.cpp \
  build/buildCornell.cpp \
  cameras/Parallel.cpp cameras/Perspective.cpp \
  geometry/Geometry.cpp geometry/Plane.cpp geometry/Sphere.cpp geometry/Traingle.cpp \
  image/Image.cpp \
  light/AreaLight.cpp light/DirectionalLight.cpp light/PointLight.cpp \
  materials/Cosine.cpp materials/Matte.cpp materials/Mirror.cpp materials/Phong.cpp \
  materials/PoolWater.cpp \
  samplers/Sampler.cpp samplers/Simple.cpp samplers/Jittered.cpp \
  tracers/Tracer.cpp tracers/Basic.cpp tracers/Shadow.cpp \
  utilities/BBox.cpp utilities/Point3d.cpp utilities/Ray.cpp utilities/RGBColor.cpp \
  utilities/ShadeInfo.cpp utilities/Vector3D.cpp \
  world/ViewPlane.cpp world/World.cpp \
  -o raytracer_lq

./raytracer_lq
```

Output is written to `scene.png` (480×360).

---

### High Quality Render (1920×1080) — `buildCornellHQ.cpp`

Swap `build/buildCornell.cpp` for `build/buildCornellHQ.cpp` in the compile command:

```bash
g++ -std=c++17 -O2 -I. \
  raytracer.cpp \
  acceleration/BVH.cpp \
  brdf/Lambertian.cpp brdf/GlossySpecular.cpp brdf/PerfectSpecular.cpp \
  build/buildCornellHQ.cpp \
  cameras/Parallel.cpp cameras/Perspective.cpp \
  geometry/Geometry.cpp geometry/Plane.cpp geometry/Sphere.cpp geometry/Traingle.cpp \
  image/Image.cpp \
  light/AreaLight.cpp light/DirectionalLight.cpp light/PointLight.cpp \
  materials/Cosine.cpp materials/Matte.cpp materials/Mirror.cpp materials/Phong.cpp \
  materials/PoolWater.cpp \
  samplers/Sampler.cpp samplers/Simple.cpp samplers/Jittered.cpp \
  tracers/Tracer.cpp tracers/Basic.cpp tracers/Shadow.cpp \
  utilities/BBox.cpp utilities/Point3d.cpp utilities/Ray.cpp utilities/RGBColor.cpp \
  utilities/ShadeInfo.cpp utilities/Vector3D.cpp \
  world/ViewPlane.cpp world/World.cpp \
  -o raytracer_hq

./raytracer_hq
```

Output is written to `scene.png` (1920×1080).

---

## Acceleration Structure Flag

In both `build/buildCornell.cpp` and `build/buildCornellHQ.cpp`, the last lines of `World::build()` are:

```cpp
accel_ptr        = new BVH(geometry);
use_acceleration = true;   // <- set to false to disable BVH
```

Set `use_acceleration = false` (and comment out the `BVH` construction) to render using brute-force ray-object intersection. In `world/World.cpp`, `hit_objects()` checks this flag and dispatches accordingly. This flag is used to benchmark BVH vs brute-force performance.

---

## Features Implemented

### Appearance — `brdf/` folder

Three BRDF classes implement distinct reflection models:

| Class | File | Model |
|---|---|---|
| `Lambertian` | `brdf/Lambertian.hpp/cpp` | Diffuse: f = kd * cd / pi |
| `GlossySpecular` | `brdf/GlossySpecular.hpp/cpp` | Phong glossy: f = ks * cs * (r.wo)^exp |
| `PerfectSpecular` | `brdf/PerfectSpecular.hpp/cpp` | Mirror reflection via sample_f |

Four `Material` subclasses use these BRDFs:

- **`Matte`** — uses `Lambertian` for ambient + diffuse shading
- **`Phong`** — uses `Lambertian` + `GlossySpecular` for diffuse + specular highlights
- **`Mirror`** — uses `PerfectSpecular` for recursive mirror reflections
- **`PoolWater`** — blends `PerfectSpecular` reflection with a transmitted ray for a semi-transparent water look

---

### Lighting — `light/` folder

Three light types are implemented under a `Light` base class:

- **`PointLight`** — light at a position, with colour and intensity
- **`DirectionalLight`** — infinite directional light, used as warm ambient fill
- **`AreaLight`** — rectangular emitter that produces soft shadows with penumbras by casting multiple shadow rays to random points on the rectangle

All lights are stored in `World::lights` (`std::vector<Light*>`).

---

### Acceleration — `acceleration/` folder

A **BVH (Bounding Volume Hierarchy)** is implemented in `acceleration/BVH.hpp/cpp`.

**To enable/disable:** set `use_acceleration = true/false` in the build file (see above).

---

### Ray Casting — `tracers/` folder

A `Tracer` hierarchy is used for all shading:

- **`Basic`** — shades with primary rays only (no shadows)
- **`ShadowTracer`** — shoots shadow rays toward each light; objects in shadow receive only ambient light. Detects `AreaLight` via `is_area_light()` and uses multi-sample soft shadow path automatically.

`World::tracer_ptr` holds the active tracer. To switch, change the build file:

```cpp
tracer_ptr = new ShadowTracer(this);   // with shadows (default)
// tracer_ptr = new Basic(this);       // without shadows
```

---

### Sampling — `samplers/` folder

- **`Simple`** — one ray per pixel
- **`Jittered`** — stratified anti-aliasing: each pixel is divided into an n x n grid of sub-cells; one ray is shot through a random point within each sub-cell. Both renders use n=4 (16 samples/pixel).

---

## Scene — Cornell Box Pool

A Cornell box containing a pool of water viewed from outside, looking in.

### Primitives

The water surface is a **710 x 711 triangle mesh = 1,010,220 triangles**, meeting the 1 million primitive requirement. Additional geometry (tiles, walls, spheres, pyramid, ripple rings) adds several thousand more primitives.

### Contents

| Element | Material | Details |
|---|---|---|
| Ceiling | Matte (vivid blue) | Double-sided quad |
| Floor | Matte grout + indigo tile | 12x22 checkerboard tile grid |
| Left/right/back walls | Matte (purple, plum, blue-slate) | Upper half above waterline |
| Pool walls | Matte grout + indigo tile | Tiled below waterline |
| Water surface | PoolWater | 710x711 sine-wave ripple mesh (1,010,220 triangles) |
| Silver sphere | Mirror | PerfectSpecular, silver tint |
| Ruby sphere | Phong | High specular exponent |
| Emerald sphere | Phong | High specular exponent |
| Orange ball | Phong | Half-submerged at impact point |
| Impact ripples | PoolWater | 4 concentric triangulated rings |
| Pyramid | Phong (two shades) | 4-faced, base visible through water |

### Lights

- **AreaLight** — warm golden rectangle near ceiling (16 shadow samples, produces soft penumbras)
- **PointLight x2** — cool blue fills from left and right walls
- **DirectionalLight** — warm soft ambient from above, lifts shadow floors

### Build Files

| Image | Build file |
|---|---|
| Low quality (480x360) | `build/buildCornell.cpp` |
| High quality (1920x1080) | `build/buildCornellHQ.cpp` |