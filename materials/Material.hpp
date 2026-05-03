#pragma once

class RGBColor;
class ShadeInfo;

class Material {
public:
  Material() = default;
  Material(const Material &other) = default;
  Material &operator=(const Material &other) = default;
  virtual ~Material() = default;

  virtual RGBColor shade(const ShadeInfo &sinfo) const = 0;

  // True for purely reflective materials — shadow tracer passes these through unscaled.
  virtual bool is_reflective() const { return false; }
};
