#pragma once
#include <cmath>

namespace cad {

struct Vector2 {
    double x, y;

    Vector2() : x(0), y(0) {}
    Vector2(double x, double y) : x(x), y(y) {}

    Vector2 operator+(const Vector2& v) const { return {x + v.x, y + v.y}; }
    Vector2 operator-(const Vector2& v) const { return {x - v.x, y - v.y}; }
    Vector2 operator*(double s) const { return {x * s, y * s}; }
    Vector2 operator/(double s) const { return {x / s, y / s}; }

    double dot(const Vector2& v) const { return x*v.x + y*v.y; }
    double length() const { return std::sqrt(x*x + y*y); }

    Vector2 normalized() const {
        double len = length();
        return (len == 0) ? Vector2(0,0) : (*this / len);
    }
};

}
