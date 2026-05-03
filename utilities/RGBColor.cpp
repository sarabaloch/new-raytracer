#include "RGBColor.hpp"

#include <cmath>
#include <string>

// clamp to 0,1
static float clamp(float x) {
    if (x < 0.0f) return 0.0f;
    if (x > 1.0f) return 1.0f;
    return x;
}

RGBColor::RGBColor() : r(0.0f), g(0.0f), b(0.0f) {}

RGBColor::RGBColor(float c) : r(clamp(c)), g(clamp(c)), b(clamp(c)) {}

RGBColor::RGBColor(float _r, float _g, float _b) : r(clamp(_r)), g(clamp(_g)), b(clamp(_b)) {}


std::string RGBColor::to_string() const {
    return "(" + std::to_string(r) + ", " + std::to_string(g) + ", " + std::to_string(b) + ")";
}

// Addition
RGBColor RGBColor::operator+(const RGBColor &c) const {
    return RGBColor(
        clamp(r + c.r),
        clamp(g + c.g),
        clamp(b + c.b)
    );
}

// compound add
RGBColor& RGBColor::operator+=(const RGBColor &c) {
    r = clamp(r + c.r);
    g = clamp(g + c.g);
    b = clamp(b + c.b);
    return *this;
}

// multiply
RGBColor RGBColor::operator*(const float a) const {
    return RGBColor(
        clamp(r * a),
        clamp(g * a),
        clamp(b * a)
    );
}

// Compound multiply
RGBColor& RGBColor::operator*=(const float a) {
    r = clamp(r * a);
    g = clamp(g * a);
    b = clamp(b * a);
    return *this;
}


// div
RGBColor RGBColor::operator/(const float a) const {
    return RGBColor(
        clamp(r / a),
        clamp(g / a),
        clamp(b / a)
    );
}

RGBColor& RGBColor::operator/=(const float a) {
    r = clamp(r / a);
    g = clamp(g / a);
    b = clamp(b / a);
    return *this;
}


// component-wise multiply
RGBColor RGBColor::operator*(const RGBColor &c) const {
    return RGBColor(
        clamp(r * c.r),
        clamp(g * c.g),
        clamp(b * c.b)
    );
}


// Equality
bool RGBColor::operator==(const RGBColor &c) const {
    return (r == c.r && g == c.g && b == c.b);
}


// component^power
RGBColor RGBColor::powc(float p) const {
    return RGBColor(
        clamp(std::pow(r, p)),
        clamp(std::pow(g, p)),
        clamp(std::pow(b, p))
    );
}

// avg
float RGBColor::average() const {
    return (r + g + b) / 3.0f;
}

//float*color
RGBColor operator*(const float a, const RGBColor &c) {
    return c * a;
}