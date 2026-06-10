#pragma once
#include <cmath>
#include "Vector3.h"

namespace cad {

struct Quaternion {
    double w, x, y, z;

    Quaternion() : w(1), x(0), y(0), z(0) {}
    Quaternion(double w, double x, double y, double z)
        : w(w), x(x), y(y), z(z) {}

    static Quaternion fromAxisAngle(const Vector3& axis, double radians) {
        double half = radians * 0.5;
        double s = std::sin(half);
        return {
            std::cos(half),
            axis.x * s,
            axis.y * s,
            axis.z * s
        };
    }

    Quaternion normalized() const {
        double len = std::sqrt(w*w + x*x + y*y + z*z);
        return { w/len, x/len, y/len, z/len };
    }
};

}
