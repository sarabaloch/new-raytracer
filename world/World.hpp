#pragma once

#include <vector>

#include "../utilities/RGBColor.hpp"
#include "ViewPlane.hpp"

// ===== forward declarations (IMPORTANT) =====
class Camera;
class Geometry;
class Ray;
class Sampler;
class ShadeInfo;
class Light;
class Tracer;
class Acceleration;

class World {
public:
  ViewPlane vplane;
  RGBColor bg_color;

  std::vector<Geometry *> geometry;
  std::vector<Light *> lights;

  Camera *camera_ptr;
  Sampler *sampler_ptr;

  Tracer *tracer_ptr;
  Acceleration *accel_ptr;

  bool use_acceleration;

public:
  World();
  ~World();

  void add_geometry(Geometry *geom_ptr);
  void set_camera(Camera *c_ptr);

  void add_light(Light* l) { lights.push_back(l); }

  void build();

  ShadeInfo hit_objects(const Ray &ray);
};