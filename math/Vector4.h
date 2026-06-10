#pragma once

namespace cad {

struct Vector4 {
    double x, y, z, w;

    Vector4() : x(0), y(0), z(0), w(0) {}
    Vector4(double x, double y, double z, double w)
        : x(x), y(y), z(z), w(w) {}
};

}
