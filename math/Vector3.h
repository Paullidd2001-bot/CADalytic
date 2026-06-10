#pragma once
#include <cmath>

namespace cad {

struct Vector3 {
    double x, y, z;

    Vector3() : x(0), y(0), z(0) {}
    Vector3(double x, double y, double z) : x(x), y(y), z(z) {}

    Vector3 operator+(const Vector3& v) const { return {x + v.x, y + v.y, z + v.z}; }
    Vector3 operator-(const Vector3& v) const { return {x - v.x, y - v.y, z - v.z}; }
    Vector3 operator*(double s) const { return {x * s, y * s, z * s}; }
    Vector3 operator/(double s) const { return {x / s, y / s, z / s}; }

    double dot(const Vector3& v) const { return x*v.x + y*v.y + z*v.z; }

    Vector3 cross(const Vector3& v) const {
        return {
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x
        };
    }

    double length() const { return std::sqrt(x*x + y*y + z*z); }

    Vector3 normalized() const {
        double len = length();
        return (len == 0) ? Vector3(0,0,0) : (*this / len);
    }
};

}
